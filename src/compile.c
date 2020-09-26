#include "compile.h"

#include <binaryen-c.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "compile_fn_call.h"
#include "compile_special.h"
#include "datafy.h"
#include "env.h"
#include "eval.h"
#include "fns.h"
#include "inter.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "misc_fns.h"
#include "namespace.h"
#include "print.h"
#include "read.h"
#include "refcount.h"
#include "run.h"
#include "runtime.h"
#include "special_fns.h"
#include "state.h"
#include "wasm.h"
#include "wasm_util.h"

CResult lval_compile(Wasm* wasm, Lval* lval);

// This list can be released. It will also release all its elements
CResult compile_list(Wasm* wasm, Cell* head) {
  BinaryenModuleRef module = wasm->module;
  if (!head) return cresult(make_int32(module, 0));
  CResult result = lval_compile(wasm, head->car);
  Ber compiled_arg =
      result.is_fn_call ? result.ber : wasm_retain(wasm, result.ber).ber;
  Ber operands[] = {compiled_arg,
                    compile_list(wasm, head->cdr).ber};  // recursive call:
  return cresult(
      BinaryenCall(module, "prefix_list", operands, 2, make_type_int32(1)));
}

CResult compile_do_list(Wasm* wasm, Lval* lval_list) {
  /* printf("COMPILE_DO_LIST!!!!\n"); */
  /* lval_println(lval_list); */
  BinaryenModuleRef module = wasm->module;
  int do_list_count = list_count(lval_list->head) + 2;  // room for extra bers

  LocalIndices* li = li_init();

  if (do_list_count < 2) do_list_count = 2;
  Ber* do_body = malloc(do_list_count * sizeof(Ber));

  int do_list_index = 0;

  /* if (init_rest_arg) do_body[do_list_index++] = init_rest_arg; */
  /* if (args_into_locals) do_body[do_list_index++] = args_into_locals; */

  Ber do_result = NULL;

  Cell* list = lval_list->head;
  if (list) {
    do {
      CResult result = lval_compile(wasm, list->car);

      Ber ber = result.ber;
      if (list->cdr) {
        // Not last value, so we drop it
        if (result.is_fn_call) {
          // And release it if it's a fn call
          int local_index = li_track(wasm, li, li_new(wasm));
          ber = BinaryenLocalSet(module, local_index, ber);
        } else {
          ber = BinaryenDrop(module, ber);
        }

        do_body[do_list_index++] = ber;
      } else {
        // ber is the last value in the body of let
        do_result = ber;
        if (!result.is_fn_call) {
          Ber retain_operand[1] = {do_result};
          // Retain value when not result of fn call (a literal)
          do_result = BinaryenCall(module, "retain", retain_operand, 1,
                                   BinaryenTypeInt32());
        }
      }

      /* if (result->type == LVAL_ERR) { */
      /*   lval_print(lval); */
      /*   printf(" resulted in: "); */
      /*   lval_println(result); */
      /* } */
      /* return result; */
      /* lval = iter_next(i); */
      list = list->cdr;
    } while (list);
  } else {
    do_result = datafy_nil(wasm).ber;
    Ber retain_operand[1] = {do_result};
    do_result =
        BinaryenCall(module, "retain", retain_operand, 1, BinaryenTypeInt32());
  }

  Ber release_locals_block = li_release(wasm, li, "release_locals_for_do").ber;

  do_body[do_list_index++] =
      li_result_with_release(wasm, li, "do_body_result", release_locals_block,
                             do_result)
          .ber;

  Ber ret = BinaryenBlock(module, uniquify_name(wasm, "do"), do_body,
                          do_list_index, BinaryenTypeInt32());
  li_close(li);
  free(do_body);

  return cresult(ret);
}

// A lval_ref is a value that we don't know at compile time, but that we
// can retrieve at runtime, because it's either a local var, a param or a closed
// over var. So we insert the code to do this in place of the symbol.
CResult compile_lval_ref(Wasm* wasm, Lval* lval_symbol, Lval* lval_ref) {
  /* Lval* lval_wasm_ref = lval_resolved_sym; */
  /* printf( */
  /*     "We've got a LVAL_REF (param, local or closed over binding)!!! "
   */
  /*     "%s\n", */
  /*     lval_symbol->str); */
  /* lval_println(lval_wasm_ref); */
  /* BinaryenModuleRef module = wasm->module; */
  Context* context = wasm->context->car;
  int is_local_ref =
      context->function_context == lval_ref->context->function_context;
  /* print_wasm_context(wasm); */
  /* print_context(lval_wasm_ref->context); */
  /* printf("------------\n"); */
  if (is_local_ref) {
    switch (lval_ref->subtype) {
      case PARAM: {
        Ber local = BinaryenLocalGet(wasm->module, lval_ref->offset,
                                     BinaryenTypeInt32());
        return cresult(local);
      }
      case LOCAL: {
        Ber local = BinaryenLocalGet(wasm->module, lval_ref->offset,
                                     BinaryenTypeInt32());
        return cresult(local);
      }
      default:
        quit(wasm, "ERROR: Unknown lval_ref subtype: %d", lval_ref->subtype);
        return cnull();
    }
  } else {  // ref to closure val
    Lval* closure_lval =
        lenv_get_or_error(context->function_context->closure, lval_symbol);
    int closure_offset;
    if (closure_lval->type == LVAL_ERR) {
      Lval* lval_closure_sym = make_lval_sym(lval_symbol->str);
      closure_offset = lval_closure_sym->offset =
          context->function_context->closure_count++;
      lenv_put(context->function_context->closure, lval_symbol, lval_ref);
    } else {
      closure_offset = closure_lval->offset;
    }
    Ber closure_pointer = BinaryenLocalGet(
        wasm->module, 0,
        BinaryenTypeInt32());  // first param to fn is closure pointer
    int _signed = 0;
    int _aligned = 2;
    Ber load_closure_lval =
        BinaryenLoad(wasm->module, 4, _signed, closure_offset * 4, _aligned,
                     BinaryenTypeInt32(), closure_pointer);

    return cresult(load_closure_lval);
  }
}

FunctionData add_wasm_function(Wasm* wasm, Lenv* env, char* fn_name,
                               Lval* lval_fun) {
  int param_count = lval_fun->param_count;
  int wajure_params_count = 1;
  int wasm_params_count =
      wajure_params_count + param_count;  // the function's closure
  int results_count = 1;
  CONTEXT_FUNCTION(">>>>>>>add_wasm_function", fn_name, wasm_params_count)

  Lenv* current_env = wasm->env;
  wasm->env = env;

  // Params
  Lenv* params_env = enter_env(wasm);

  int rest_arg_index = lval_fun->rest_arg_index;  // 1 based
  int min_param_count = rest_arg_index ? param_count - 1 : param_count;

  int i = 0;
  Cell* param = lval_fun->params->head;
  while (i < min_param_count) {
    Lval* lval_ref = make_lval_ref(context, PARAM, i + wajure_params_count);
    Lval* lval_sym = param->car;
    lenv_put(params_env, lval_sym, lval_ref);
    release(lval_ref);
    param = param->cdr;
    i++;
  }

  if (rest_arg_index) {
    param = param->cdr;
    Lval* lval_ref = make_lval_ref(context, PARAM, i + wajure_params_count);
    Lval* lval_sym = param->car;
    lenv_put(params_env, lval_sym, lval_ref);
  }
  /* env_print(params_env); */

  // Compile body
  Ber body = compile_do_list(wasm, lval_fun->body).ber;

  // Post compile
  leave_env(wasm);

  int locals_count = context->function_context->local_count - wasm_params_count;

  int fn_table_index = add_fn_to_table(wasm, fn_name);
  lval_fun->offset = fn_table_index;
  lval_fun->param_count = param_count;
  lval_fun->rest_arg_index = rest_arg_index;

  BinaryenModuleRef module = wasm->module;

  BinaryenType params_type = make_type_int32(wasm_params_count);
  BinaryenType results_type = make_type_int32(results_count);

  BinaryenType* local_types = make_type_int32_array(locals_count);

  // Add fn to wasm
  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, body);
  FunctionData function_data = {
      fn_table_index, retain(context->function_context->closure),
      context->function_context->closure_count, param_count, rest_arg_index};

  wasm->env = current_env;
  return function_data;
}

// Eval the form first to get params and body, and add a wasm fn to our
// compiled code. This gives us info on the fn's wasm fn table index, whether
// it has a rest arg and a map of free variables (closed over variables).
// These are vars that couldn't be found in the local fn's env.

// We then add code that builds at runtime an lval of type WVAL_FUN
// that holds all info on the lambda so that it can be called. We add to this
// lval at runtime a closure data block that holds values of the free vars
// found in the fn.
CResult compile_local_lambda(Wasm* wasm, Cell* args) {
  BinaryenModuleRef module = wasm->module;

  Context* context = wasm->context->car;
  Lval* arg_list = new_lval_list(retain(args));

  // Eval the lambda form to get info on params and body
  Lval* lval_fun = eval_lambda_form(wasm->env, arg_list, LAMBDA);
  release(arg_list);

  /* char* lambda_name = lalloc_size(512); */
  /* _strcpy(lambda_name, context->function_context->fn_name); */
  /* _strcat(lambda_name, "#"); */
  /* itostr(lambda_name + _strlen(lambda_name), wasm->fns_count + 1); */
  char* lambda_name = number_fn_name(wasm, context->function_context->fn_name);

  FunctionData function_data =
      add_wasm_function(wasm, wasm->env, lambda_name, lval_fun);

  printf("COMPILED FUNCTION!!! %d %d %d\n", function_data.fn_table_index,
         function_data.closure_count, function_data.param_count);

  release(lval_fun);
  Ber fn_table_index =
      make_int32(module, wasm->__fn_table_end + function_data.fn_table_index);
  if (wasm->pic) {
    Ber fn_table_offset =
        BinaryenGlobalGet(wasm->module, "fn_table_offset", BinaryenTypeInt32());
    fn_table_index = BinaryenBinary(wasm->module, BinaryenAddInt32(),
                                    fn_table_offset, fn_table_index);
  }
  Ber partial_count = make_int32(module, 0);
  Ber partials = make_int32(module, 0);  // NULL
  Ber lispy_param_count = make_int32(module, function_data.param_count);
  Ber wasm_has_rest_arg = make_int32(module, function_data.has_rest_arg);

  Ber* lambda_list = malloc((function_data.closure_count + 2) * sizeof(Ber));

  printf("closure from compiled fn:\n");
  env_print(function_data.closure);
  Cell* closure_cell = function_data.closure->kv;
  int lambda_block_count = 0;

  // Allocate a block of memory to hold the closure for the lambda, and store
  // pointer in local var
  Ber wasm_closure_pointer =
      wasm_lalloc_size(wasm, function_data.closure_count * 4).ber;
  int closure_pointer_local_index = context->function_context->local_count;
  Ber local_set = BinaryenLocalSet(module, closure_pointer_local_index,
                                   wasm_closure_pointer);
  context->function_context->local_count++;
  lambda_list[lambda_block_count++] = local_set;

  wasm_closure_pointer = BinaryenLocalGet(module, closure_pointer_local_index,
                                          BinaryenTypeInt32());

  // Store lval pointers in the closure memory block
  int offset = function_data.closure_count * 4;
  while (closure_cell) {
    Cell* pair = closure_cell->car;
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;
    Ber closure_lval_pointer = compile_lval_ref(wasm, lval_sym, lval).ber;
    offset -= 4;

    Ber store_wasm_ref =
        BinaryenStore(module, 4, offset, 0, wasm_closure_pointer,
                      closure_lval_pointer, BinaryenTypeInt32());
    lambda_list[lambda_block_count++] = store_wasm_ref;
    /* lval_print(lval_sym); */
    /* printf(": %d ", lval->offset); */
    /* lval_println(lval); */
    closure_cell = closure_cell->cdr;
  }
  /* printf("Current env:\n"); */
  /* env_print(wasm->env); */

  /* printf("Current closure:\n"); */
  /* env_print(context->function_context->closure); */

  // Make a lval_wasm_lambda with info on fn table index, param count, pointer
  // to closure etc.
  partials = BinaryenLocalGet(module, 1, BinaryenTypeInt32());
  Ber operands[6] = {fn_table_index,       lispy_param_count, wasm_has_rest_arg,
                     wasm_closure_pointer, partials,          partial_count};
  Ber make_lval_wasm_lambda_call = BinaryenCall(
      wasm->module, "make_lval_wasm_lambda", operands, 6, make_type_int32(1));

  lambda_list[lambda_block_count++] = make_lval_wasm_lambda_call;

  Ber lambda_block =
      BinaryenBlock(module, uniquify_name(wasm, lambda_name), lambda_list,
                    lambda_block_count, BinaryenTypeAuto());
  free(lambda_list);
  release(lambda_name);

  CResult ret = {.ber = lambda_block, .is_fn_call = 1};
  return ret;
}

CResult compile_special_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  /* printf("compile special call!!!!!!!!!!!!!!!\n"); */
  char* fn_name = lval_fn_sym->str;
  if (_strcmp(fn_name, "let") == 0) return compile_let(wasm, args);
  if (_strcmp(fn_name, "if") == 0) return compile_if(wasm, args);
  if (_strcmp(fn_name, "do") == 0) {
    Lval* do_list = make_lval_list();
    do_list->head = retain(args);
    CResult result = compile_do_list(wasm, do_list);
    release(do_list);
    result.is_fn_call = 1;
    return result;
  }

  if (_strcmp(fn_name, "fn") == 0) return compile_local_lambda(wasm, args);
  if (_strcmp(fn_name, "try") == 0) return compile_try(wasm, args);
  if (_strcmp(fn_name, "throw") == 0) return compile_throw(wasm, args);

  /* if (_strcmp(fn_name, "loop") == 0) return compile_loop(wasm, args); */
  /* if (_strcmp(fn_name, "recur") == 0) return compile_recur(wasm, args); */

  if (_strcmp(fn_name, "quote") == 0) return compile_quote(wasm, args);
  if (_strcmp(fn_name, "quasiquote") == 0)
    return compile_quasiquote(wasm, args);
  quit(wasm, "ERROR: Unknown special form: %s", fn_name);
  return cnull();
}

CResult compile_partial_call(Wasm* wasm, NativeFn native_fn, Cell* args) {
  BinaryenModuleRef module = wasm->module;
  int args_count = list_count(args);
  if (args_count == 0) quit(wasm, "Need at last one argument for partial");
  Cell* head = args;
  CResult first_arg = lval_compile(wasm, head->car);

  printf("-------------partial first arg------is_fn_call %d\n ",
         first_arg.is_fn_call);
  lval_println(first_arg.lval);

  head = head->cdr;
  // If just one arg, return compiled arg
  if (!head) {
    return first_arg;
  }

  if (first_arg.is_fn_call || first_arg.lval->type == LVAL_REF) {
    // Put the compiled args in a block (lalloc_size) and pass them to our
    // native partial fn
    // Check for type:
    // 1. LVAL_WASM_LAMBDA -> add partials to a copy of the lval_wasm_lambda of
    // fn and args to partial, return it
    // 1. vector/set/map -> make lval_wasm_lambda of get and args to partial fn
    // as partials
    // 3. anything else: return as is

    return first_arg;  // TODO
  } else
    switch (first_arg.lval->type) {
      case LVAL_FUNCTION: {
        // If we know this is a function at compile time then we compile the
        // rest of the args and put them in the partials block of a copy of the
        // lval_wasm_lambda we know we have.
        Ber* children = malloc(sizeof(Ber) * args_count + 1);  //
        int children_count = 0;
        Ber lalloc_size_operands[] = {make_int32(module, (args_count - 1) * 4)};
        Ber lalloc_partials =
            BinaryenCall(module, "lalloc_size", lalloc_size_operands, 1,
                         BinaryenTypeInt32());
        int partials_ptr_index = li_new(wasm);
        Ber set_local_to_partials_ptr =
            BinaryenLocalSet(module, partials_ptr_index, lalloc_partials);
        children[children_count++] = set_local_to_partials_ptr;
        int i = 0;
        do {
          CResult compiled_arg = lval_compile(wasm, head->car);
          // This is so that we can release all partials when releasing the
          // lval_wasm_lambda:
          if (!compiled_arg.is_fn_call)
            compiled_arg.ber = wasm_retain(wasm, compiled_arg.ber).ber;
          Ber partials_ptr =
              BinaryenLocalGet(module, partials_ptr_index, BinaryenTypeInt32());
          children[children_count++] =
              BinaryenStore(module, 4, i++ * 4, 2, partials_ptr,
                            compiled_arg.ber, BinaryenTypeInt32());
          head = head->cdr;
        } while (head);

        Ber fn_table_index = make_int32(module, first_arg.lval->offset);
        fn_table_index = BinaryenBinary(
            module, BinaryenAddInt32(), fn_table_index,
            BinaryenGlobalGet(module, "fn_table_offset", BinaryenTypeInt32()));
        Ber param_count = make_int32(module, first_arg.lval->param_count);
        Ber has_rest_arg = make_int32(module, first_arg.lval->rest_arg_index);
        Ber closure_ptr = make_int32(module, 0);
        Ber partials_ptr =
            BinaryenLocalGet(module, partials_ptr_index, BinaryenTypeInt32());
        Ber partial_count = make_int32(module, args_count - 1);
        Ber operands[6] = {fn_table_index, param_count,  has_rest_arg,
                           closure_ptr,    partials_ptr, partial_count};
        children[children_count++] =
            BinaryenCall(wasm->module, "make_lval_wasm_lambda", operands, 6,
                         make_type_int32(1));

        Ber block =
            BinaryenBlock(module, uniquify_name(wasm, "make_partial_fn"),
                          children, children_count, BinaryenTypeInt32());
        free(children);
        CResult ret = {.ber = block, .is_fn_call = 1};
        return ret;
      }
      case LVAL_COLLECTION:;
        // TODO: If we know its a vector/set/map at compile time we return a
        // lval_wasm_lambda of the sys fn get, with 2+ partials: the
        // vector/set/map and the rest of the compiled args to the partial fn.
        // Falling through to default for now..
      default: {
        // If we know we have anything that's not a result of a fn call (direct
        // or lval_ref) then we return it as is , and compile the rest
        // of the args still for side effects. So we only compile fn calls.
        Ber* children = malloc(sizeof(Ber) * args_count);  //
        int children_count = 0;
        do {
          CResult compiled_arg = lval_compile(wasm, head->car);
          // For any side effects:
          if (compiled_arg.is_fn_call) {
            compiled_arg.ber = wasm_release(wasm, compiled_arg.ber).ber;
            children[children_count++] = compiled_arg.ber;
          }
          head = head->cdr;
        } while (head);
        children[children_count++] = first_arg.ber;

        Ber block = BinaryenBlock(
            module, uniquify_name(wasm, "make_pointless_partial_fn"), children,
            children_count, BinaryenTypeInt32());
        free(children);
        return cresult(block);
      }
    }
}

CResult compile_native_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  NativeFn* native_fn =
      alist_get(wasm->wajure_to_native_fn_map, is_eq_str, lval_fn_sym->str);
  if (!native_fn) {
    quit(wasm, "Function %s not found in runtime", lval_fn_sym->str);
  }
  switch (native_fn->fn_table_index) {
    case PARTIAL_FN_INDEX:
      return compile_partial_call(wasm, *native_fn, args);
    case APPLY_FN_INDEX:
      /* return compile_apply_call(wasm, *native_fn, args); */
      /* break; */
    default:
      return quit(wasm, "Oops, can't find fn table index for %s",
                  lval_fn_sym->str);
  }
  return cnull();
}

CResult compile_sys_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  /* printf("compile sys call!!!!!!!!!!!!!!!\n"); */

  char* c_fn_name =
      alist_get(wasm->wajure_to_c_fn_map, is_eq_str, lval_fn_sym->str);

  if (!c_fn_name) return compile_native_call(wasm, lval_fn_sym, args);

  BinaryenModuleRef module = wasm->module;

  Ber arg_list_head = compile_list(wasm, args).ber;

  Ber lval_list_operands[1] = {arg_list_head};
  int wasm_lval_list_index = li_new(wasm);
  Ber wasm_lval_list = BinaryenCall(module, "new_lval_list", lval_list_operands,
                                    1, make_type_int32(1));
  wasm_lval_list = BinaryenLocalTee(module, wasm_lval_list_index,
                                    wasm_lval_list, BinaryenTypeInt32());

  Ber sys_fn_operands[2] = {make_int32(module, 0), wasm_lval_list};

  Ber sys_fn_call =
      BinaryenCall(module, c_fn_name, sys_fn_operands, 2, make_type_int32(1));
  int call_result_index = li_new(wasm);
  Ber store_call_result =
      BinaryenLocalSet(module, call_result_index, sys_fn_call);
  Ber release_list =
      wasm_release(wasm, BinaryenLocalGet(module, wasm_lval_list_index,
                                          BinaryenTypeInt32()))
          .ber;
  Ber get_call_result =
      BinaryenLocalGet(module, call_result_index, BinaryenTypeInt32());
  Ber children[3] = {store_call_result, release_list, get_call_result};
  Ber block = BinaryenBlock(module, uniquify_name(wasm, "sys_call"), children,
                            3, BinaryenTypeInt32());
  CResult result = {.ber = block, .is_fn_call = 1};
  return result;
}

CResult compile_vector(Wasm* wasm, Lval* lval) {
  Ber arg_list_head = compile_list(wasm, lval->head).ber;

  Ber lval_vector_operands[1] = {arg_list_head};
  Ber wasm_lval_vector =
      BinaryenCall(wasm->module, "new_lval_vector", lval_vector_operands, 1,
                   make_type_int32(1));

  CResult ret = {.ber = wasm_lval_vector, .is_fn_call = 1};
  return ret;
}

// This will compile an unevalled lval (so which is composed of only literals,
// symbols and collections) to wasm code that will leave a pointer to an lval
// on the stack. This time the lval can also be a lambda. Eg a symbol might
// resolve to something we know is a function (like a sys or a root fn), or a
// list is of the form (fn [..] ..).
CResult lval_compile(Wasm* wasm, Lval* lval) {
  /* printf("lval_compile: "); */
  /* lval_println(lval); */
  /* printf("lval->type: %s\n", lval_type_constant_to_name(lval->type)); */

  Lval* resolved_sym = NULL;
  char* global_name = NULL;
  switch (lval->type) {
    case LVAL_SYMBOL:;
      // Resolve symbol in our compiler env and compile it. At runtime there's
      // no notion of environments or symbols that resolve to other lvalues.
      // Symbols at runtime are just that, a literal similar to strings,
      // numbers etc.

      struct resolved_symbol result = eval_symbol(wasm->env, lval);

      resolved_sym = result.lval;
      /* printf("resolved:"); */
      /* lval_println(lval_resolved_sym); */
      /* printf("namespace: %s\n", result.ns->namespace); */
      if (result.ns) {
        global_name =
            make_global_name("data:", result.ns->namespace, result.name);
        release(result.name);
        add_dep(wasm, global_name);
        /* lval_println(lval_resolved_sym); */
      }
      /* lval_println(lval_resolved_sym); */
      switch (resolved_sym->type) {
        case LVAL_ERR:
          return quit(wasm, resolved_sym->str);
        case LVAL_REF:;
          // symbol has been added to env while compiling, so
          // it's a closure var, param or local (let) var
          CResult ret = compile_lval_ref(wasm, lval, resolved_sym);
          ret.lval = resolved_sym;
          release(resolved_sym);
          return ret;
        case LVAL_FUNCTION:  // as evalled in our compiler env
          switch (resolved_sym->subtype) {
            case MACRO:
            case LAMBDA:  // functions in compiler env
              if (resolved_sym->str == NULL)
                resolved_sym->str = retain(lval->str);

            default:;
          }
        default:
          lval = resolved_sym;
      };
      break;
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST: {
          CResult ret = compile_application(wasm, lval);  // fn call
          ret.lval = lval;
          return ret;
        }
        case VECTOR: {
          CResult ret = compile_vector(wasm, lval);  // fn call
          ret.lval = lval;
          return ret;
        }
      }
    default:;
  }

  CResult ret = datafy_lval(wasm, lval, global_name);
  ret.lval = lval;
  /* release(global_name); */
  release(resolved_sym);
  return ret;
}

void print_pair(Lval* lval_sym, Lval* lval) {
  lval_print(lval_sym);
  printf(": ");
  lval_print(lval);
}

void add_deps_custom_section(Wasm* wasm, char* custom_section_name,
                             Cell* deps) {
  int deps_str_len = 0;
  Cell* head = deps;
  while (head) {
    Cell* pair = head->car;
    deps_str_len += _strlen((char*)(pair->car)) + 1;
    head = head->cdr;
    char* str = pair->car;
    /* printf("GLOBAL IMPORT: %s %d\n", str, _strlen(str)); */

    BinaryenAddGlobalImport(wasm->module, str, "env", str, BinaryenTypeInt32(),
                            0);
  }

  char* deps_str = malloc(deps_str_len + 1);
  deps_str[0] = '\0';
  head = deps;
  char* str = deps_str;
  while (head) {
    Cell* pair = head->car;
    sprintf(str, "%s\n", pair->car);
    str = str + _strlen(pair->car) + 1;
    head = head->cdr;
  }
  BinaryenAddCustomSection(wasm->module, custom_section_name, deps_str,
                           deps_str_len);
}

void compile(Namespace* ns) {
  printf("->>>>>>>>>>>> Compiling %s <<<<<<<<<<<<<<<<<< \n", ns->namespace);
  /* scoped char* file_name = ns_to_src(ns->namespace); */
  set_log_level(LOG_LEVEL_INFO);

  /* printf("FILE_NAME: %s\n", file_name); */
  Wasm* wasm = init_wasm();

  set_current_ns(ns);
  /* Namespace* foo_ns = get_namespace("foo.core"); */
  /* printf("foo.core: %s", foo_ns->namespace); */
  Lenv* env = ns->env;

  /* env_print(env); */
  register_native_fns(wasm);
  import_runtime(wasm);

  // Since we load config->main ("main") first in nodejs that module will have
  // offset of 0 for data and fns, so we add the call fns to main and now we
  // can refer to these call fns by index 0-20 throughout all the modules.
  if (_strcmp(ns->namespace, config->main) == 0) {
    add_call_fns(wasm);        // 0-20
    add_validate_fn_fn(wasm);  // 21
    add_partial_fn(wasm);      // 22
    add_apply_fn(wasm);        // 23
  }

  printf("Processing env =============\n");
  Cell* cell = env->kv;
  while (cell) {
    Cell* pair = cell->car;
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;

    CResult result;

    printf("----Processing: ");
    lval_print(lval_sym);
    printf(": ");
    lval_println(lval);
    if (lval->type == LVAL_FUNCTION) {
      if (lval->subtype == LAMBDA) {  // not interested in compiling macros!
        lval->str = retain(lval_sym->str);
        result = lval_compile(wasm, lval);
        add_to_symbol_table(wasm, lval_sym->str, lval);
      }
    } else {
      result = datafy_lval(wasm, lval, NULL);
      add_to_symbol_table(wasm, lval_sym->str, lval);
    }

    cell = cell->cdr;
  }

  printf("===== DONE Processing env\n");

  /* add_fn_to_table(wasm, "log_int"); */
  /* add_fn_to_table(wasm, "printf_"); */

  add_function_table(wasm);

  inter_rewrite_info(wasm);
  add_memory_section(wasm);

  BinaryenAddCustomSection(wasm->module, "symbol_table", wasm->symbol_table,
                           wasm->symbol_table_count);

  add_deps_custom_section(wasm, "deps", wasm->deps);

  char* size_str = malloc(100);
  sprintf(size_str, "%d", wasm->data_offset);
  BinaryenAddCustomSection(wasm->module, "data_size", size_str,
                           _strlen(size_str));
  sprintf(size_str, "%d", wasm->fns_count);
  BinaryenAddCustomSection(wasm->module, "fn_table_size", size_str,
                           _strlen(size_str));
  free(size_str);

  printf("writing wat and wasm!!!! %s\n", ns_to_wat(ns->namespace));
  write_wat(wasm, ns_to_wat(ns->namespace));
  write_wasm(wasm, ns_to_wasm(ns->namespace));

  printf("Validating %s\n", ns_to_wasm(ns->namespace));
  int validate_result = BinaryenModuleValidate(wasm->module);
  if (!validate_result) {
    printf("VALIDATE ERROR!!!!!!\n");
    exit(1);
  } else
    printf("VALIDATED OK!!\n");
  /* BinaryenModulePrint(wasm->module); */

  free_wasm(wasm);
}

/* CResult store_args_in_locals(Wasm* wasm, Lenv* params_env, Lval**
 * lispy_params, */
/*                              int param_count) { */
/*   BinaryenModuleRef module = wasm->module; */
/*   Context* context = wasm->context->car; */
/*   Ber children[param_count + 1]; */
/*   int stack_pointer_index = li_new(wasm); */
/*   Ber stack_pointer = */
/*       BinaryenGlobalGet(wasm->module, "stack_pointer",
 * BinaryenTypeInt32());
 */

/*   int sp_adjustment = param_count * 4; */
/*   stack_pointer = BinaryenBinary(module, BinaryenSubInt32(), stack_pointer,
 */
/*                                  make_int32(module, sp_adjustment)); */
/*   Ber store_stack_pointer_in_local = */
/*       BinaryenLocalSet(module, stack_pointer_index, stack_pointer); */
/*   int children_count = 0; */
/*   children[children_count++] = store_stack_pointer_in_local; */

/*   int _signed = 0; */
/*   int _aligned = 0; */
/*   for (int i = 0; i < param_count; i++) { */
/*     Ber get_stack_pointer_from_local = */
/*         BinaryenLocalGet(module, stack_pointer_index, BinaryenTypeInt32());
 */
/*     int offset = sp_adjustment - (i + 1) * 4; */
/*     Ber load_arg_lval = */
/*         BinaryenLoad(wasm->module, 4, _signed, offset, _aligned, */
/*                      BinaryenTypeInt32(), get_stack_pointer_from_local); */
/*     int local_index = li_new(wasm); */
/*     Lval* lval_ref = make_lval_ref(context, LOCAL, local_index); */
/*     lenv_put(params_env, lispy_params[i], lval_ref); */
/*     release(lval_ref); */
/*     Ber store_in_local = BinaryenLocalSet(module, local_index,
 * load_arg_lval); */
/*     children[children_count++] = store_in_local; */
/*   } */
/*   Ber ret = BinaryenBlock(module, uniquify_name(wasm,
 * "store_args_in_locals"), */
/*                           children, children_count, BinaryenTypeNone()); */
/*   return cresult(ret); */
/* } */
