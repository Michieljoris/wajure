#include "compile.h"

#include <binaryen-c.h>
#include <stdlib.h>

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
#include "print.h"
#include "read.h"
#include "refcount.h"
#include "runtime.h"
#include "special_fns.h"
#include "wasm.h"
#include "wasm_util.h"

CResult lval_compile(Wasm* wasm, Lval* lval);

CResult compile_fn_rest_args(Wasm* wasm, Cell* head) {
  BinaryenModuleRef module = wasm->module;
  if (!head) {
    return cresult(make_int32(module, 0));
  }
  /* printf("compile fn rest args:\n"); */
  /* lval_println(head->car); */
  /* printf("compile fn rest args:\n"); */
  Ber literal = lval_compile(wasm, head->car).ber;
  Ber operands[] = {literal,
                    // recursive call:
                    compile_fn_rest_args(wasm, head->cdr).ber};
  // TODO: don't use list_cons, list_cons version that doesn't retain cells, but
  // does still retain its car
  return cresult(
      BinaryenCall(module, "list_cons", operands, 2, make_type_int32(1)));
}

CResult compile_do_list(Wasm* wasm, Ber init_rest_arg, Ber args_into_locals,
                        Lval* lval_list) {
  printf("COMPILE_DO_LIST!!!!\n");
  /* lval_println(lval_list); */
  BinaryenModuleRef module = wasm->module;
  int do_list_count = list_count(lval_list->head) + 2;  // room for extra bers

  LocalIndices* li = li_init();

  if (do_list_count < 2) do_list_count = 2;
  Ber* do_body = malloc(do_list_count * sizeof(Ber));

  int do_list_index = 0;

  if (init_rest_arg) do_body[do_list_index++] = init_rest_arg;
  if (args_into_locals) do_body[do_list_index++] = args_into_locals;

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

// A lval_compiler is a value that we don't know at compile time, but that we
// can retrieve at runtime. So we insert this code in place of the symbol.
CResult compile_lval_compiler(Wasm* wasm, Lval* lval_symbol,
                              Lval* lval_compiler) {
  /* Lval* lval_wasm_ref = lval_resolved_sym; */
  printf(
      "We've got a LVAL_COMPILER (param, local or closed over binding)!!! "
      "%s\n",
      lval_symbol->str);
  /* lval_println(lval_wasm_ref); */
  BinaryenModuleRef module = wasm->module;
  Context* context = wasm->context->car;
  int is_local_ref =
      context->function_context == lval_compiler->context->function_context;
  /* print_wasm_context(wasm); */
  /* print_context(lval_wasm_ref->context); */
  /* printf("------------\n"); */
  if (is_local_ref) {
    switch (lval_compiler->subtype) {
      case PARAM:;
        Ber stack_pointer = BinaryenGlobalGet(wasm->module, "stack_pointer",
                                              BinaryenTypeInt32());
        int param_count = context->function_context->param_count;

        int sp_adjustment = param_count * 4;
        stack_pointer =
            BinaryenBinary(module, BinaryenSubInt32(), stack_pointer,
                           make_int32(module, sp_adjustment));
        int _signed = 0;
        int _aligned = 0;

        int offset = sp_adjustment - (lval_compiler->offset + 1) * 4;
        Ber load_arg_lval =
            BinaryenLoad(wasm->module, 4, _signed, offset, _aligned,
                         BinaryenTypeInt32(), stack_pointer);
        return cresult(load_arg_lval);
      case LOCAL:;
        Ber local = BinaryenLocalGet(wasm->module, lval_compiler->offset,
                                     BinaryenTypeInt32());
        return cresult(local);
      default:
        quit(wasm, "ERROR: Unknown lval_compiler subtype: %d",
             lval_compiler->subtype);
        return cnull();
    }
  } else {  // ref to closure val
    Lval* closure_lval =
        lenv_get(context->function_context->closure, lval_symbol);
    int closure_offset;
    if (closure_lval->type == LVAL_ERR) {
      Lval* lval_closure_sym = make_lval_sym(lval_symbol->str);
      closure_offset = lval_closure_sym->offset =
          context->function_context->closure_count++;
      lenv_put(context->function_context->closure, retain(lval_symbol),
               retain(lval_compiler));
    } else {
      closure_offset = closure_lval->offset;
    }
    Ber closure_pointer = BinaryenLocalGet(
        wasm->module, 0,
        BinaryenTypeInt32());  // first param to fn is closure pointer
    int _signed = 0;
    int _aligned = 0;
    Ber load_closure_lval =
        BinaryenLoad(wasm->module, 4, _signed, closure_offset * 4, _aligned,
                     BinaryenTypeInt32(), closure_pointer);

    return cresult(load_closure_lval);
  }
}

enum { RT_TOO_FEW_ARGS, RT_TOO_MANY_ARGS };

CResult wasm_process_args(Wasm* wasm, int param_count, int rest_arg_index) {
  BinaryenModuleRef module = wasm->module;
  Context* context = wasm->context->car;
  char* fn_name = context->function_context->fn_name;
  Ber args_count = BinaryenLocalGet(
      wasm->module, 1,
      BinaryenTypeInt32());  // second param to fn is args count

  Ber check_args_count = NULL;
  if (wasm->runtime_check_args_count && rest_arg_index != 1) {
    Ber wasm_param_count = make_int32(module, param_count);
    Ber check_operands[3] = {wasm_param_count, args_count,
                             make_int32(module, rest_arg_index)};
    Ber check_args_count_call_result = BinaryenCall(
        module, "check_args_count", check_operands, 3, BinaryenTypeInt32());
    BinaryenOp eq = BinaryenEqInt32();
    Ber is_too_few_args =
        BinaryenBinary(module, eq, check_args_count_call_result,
                       make_int32(module, TOO_FEW_ARGS));
    Ber is_too_many_args =
        BinaryenBinary(module, eq, check_args_count_call_result,
                       make_int32(module, TOO_MANY_ARGS));
    Ber all_ok = BinaryenNop(module);
    Ber if_too_few = BinaryenIf(
        module, is_too_few_args,
        wasm_runtime_error(wasm, RT_TOO_FEW_ARGS, fn_name).ber, all_ok);
    Ber if_too_many = BinaryenIf(
        module, is_too_many_args,
        wasm_runtime_error(wasm, RT_TOO_MANY_ARGS, fn_name).ber, if_too_few);

    check_args_count = if_too_many;
  }

  Ber init_rest_arg = NULL;
  if (rest_arg_index) {
    Ber offset = BinaryenBinary(module, BinaryenMulInt32(),
                                make_int32(module, 4), args_count);

    Ber stack_pointer =
        BinaryenGlobalGet(wasm->module, "stack_pointer", BinaryenTypeInt32());
    Ber args_start =
        BinaryenBinary(module, BinaryenSubInt32(), stack_pointer, offset);

    Ber rest_arg_length =
        BinaryenBinary(module, BinaryenSubInt32(), args_count,
                       make_int32(module, rest_arg_index - 1));
    Ber operands[] = {args_start, rest_arg_length};
    init_rest_arg =
        BinaryenCall(module, "init_rest_args", operands, 2, BinaryenTypeNone());
    if (check_args_count == NULL) return cresult(init_rest_arg);
  }
  if (check_args_count) {
    if (!init_rest_arg) return cresult(check_args_count);
    Ber block_children[2] = {check_args_count, init_rest_arg};
    return cresult(BinaryenBlock(module, "process_args", block_children, 2,
                                 BinaryenTypeNone()));
  }
  return cnull();
}

CResult store_args_in_locals(Wasm* wasm, Lenv* params_env, Lval** lispy_params,
                             int param_count) {
  BinaryenModuleRef module = wasm->module;
  Context* context = wasm->context->car;
  Ber children[param_count + 1];
  int stack_pointer_index = li_new(wasm);
  Ber stack_pointer =
      BinaryenGlobalGet(wasm->module, "stack_pointer", BinaryenTypeInt32());

  int sp_adjustment = param_count * 4;
  stack_pointer = BinaryenBinary(module, BinaryenSubInt32(), stack_pointer,
                                 make_int32(module, sp_adjustment));
  Ber store_stack_pointer_in_local =
      BinaryenLocalSet(module, stack_pointer_index, stack_pointer);
  int children_count = 0;
  children[children_count++] = store_stack_pointer_in_local;

  int _signed = 0;
  int _aligned = 0;
  for (int i = 0; i < param_count; i++) {
    Ber get_stack_pointer_from_local =
        BinaryenLocalGet(module, stack_pointer_index, BinaryenTypeInt32());
    int offset = sp_adjustment - (i + 1) * 4;
    Ber load_arg_lval =
        BinaryenLoad(wasm->module, 4, _signed, offset, _aligned,
                     BinaryenTypeInt32(), get_stack_pointer_from_local);
    int local_index = li_new(wasm);
    lenv_put(params_env, retain(lispy_params[i]),
             make_lval_compiler(context, LOCAL, local_index));
    Ber store_in_local = BinaryenLocalSet(module, local_index, load_arg_lval);
    children[children_count++] = store_in_local;
  }
  Ber ret = BinaryenBlock(module, uniquify_name(wasm, "store_args_in_locals"),
                          children, children_count, BinaryenTypeNone());
  return cresult(ret);
}

FunctionData add_wasm_function(Wasm* wasm, Lenv* env, char* fn_name,
                               Lval* lval_fun) {
  int wasm_params_count = 2;  // the function's closure and args count
  int results_count = 1;
  CONTEXT_FUNCTION(">>>>>>>add_wasm_function", fn_name, wasm_params_count)

  Lenv* current_env = wasm->env;
  wasm->env = env;

  // Params
  Lenv* params_env = enter_env(wasm);
  Cell* param = lval_fun->params->head;
  // Includes any rest arg, so [a & b] has lispy_param_count = 2
  int lispy_param_count = 0;
  int rest_arg_index = 0;
  int offset = 0;
  Lval** lispy_params = malloc(list_count(param) * sizeof(Lval));
  while (param) {
    if (rest_arg_index && lispy_param_count == rest_arg_index) {
      lval_println(lval_fun);
      quit(wasm, "ERROR: only one rest arg allowed");
    }
    Lval* lval_sym = param->car;
    if (_strcmp(lval_sym->str, "&") == 0) {
      rest_arg_index = offset + 1;  // number of params
      param = param->cdr;
      continue;
    }
    offset++;
    lispy_params[lispy_param_count++] = lval_sym;
    param = param->cdr;
  }
  context->function_context->param_count = lispy_param_count;

  if (rest_arg_index && lispy_param_count != rest_arg_index)
    quit(wasm, "ERROR: rest arg missing");

  Ber process_args =
      wasm_process_args(wasm, lispy_param_count, rest_arg_index).ber;
  Ber wasm_args_into_locals =
      store_args_in_locals(wasm, params_env, lispy_params, lispy_param_count)
          .ber;
  free(lispy_params);

  // Compile body
  Ber body =
      compile_do_list(wasm, process_args, wasm_args_into_locals, lval_fun->body)
          .ber;

  // Post compile
  leave_env(wasm);

  int locals_count = context->function_context->local_count - wasm_params_count;

  int fn_table_index = add_fn_to_table(wasm, fn_name);
  lval_fun->offset = fn_table_index;

  BinaryenModuleRef module = wasm->module;

  BinaryenType params_type = make_type_int32(wasm_params_count);
  BinaryenType results_type = make_type_int32(results_count);

  BinaryenType* local_types = make_type_int32_array(locals_count);

  // Add fn to wasm
  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, body);

  FunctionData function_data = {fn_table_index,
                                retain(context->function_context->closure),
                                context->function_context->closure_count,
                                lispy_param_count, rest_arg_index};

  wasm->env = current_env;
  return function_data;
}

// Eval the form first to get params and body, and add a wasm fn to our
// compiled code. This gives us info on the fn's wasm fn table index, whether
// it has a rest arg and a map of free variables (closed over variables).
// These are vars that couldn't be found in the local fn's env.

// We then add code that builds at runtime an lval of type LVAL_WASM_LAMBDA
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

  char* lambda_name = lalloc_size(512);
  _strcpy(lambda_name, context->function_context->fn_name);
  _strcat(lambda_name, "#");
  itostr(lambda_name + _strlen(lambda_name), wasm->fns_count + 1);

  FunctionData function_data =
      add_wasm_function(wasm, wasm->env, lambda_name, lval_fun);

  /* printf("COMPILED FUNCTION!!! %d %d\n", function_data.fn_table_index, */
  /*        function_data.closure_count); */

  release(lval_fun);

  Ber fn_table_index = make_int32(module, function_data.fn_table_index);
  Ber partial_count = make_int32(module, 0);
  Ber partials = make_int32(module, 0);  // NULL
  Ber lispy_param_count = make_int32(module, function_data.param_count);
  Ber wasm_has_rest_arg = make_int32(module, function_data.has_rest_arg);

  Ber* lambda_list = malloc((function_data.closure_count + 2) * sizeof(Ber));

  /* env_print(function_data.closure); */
  /* printf("closure from compiled fn:\n"); */
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
    Ber closure_lval_pointer = compile_lval_compiler(wasm, lval_sym, lval).ber;
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
  Ber operands[6] = {fn_table_index,       lispy_param_count, wasm_has_rest_arg,
                     wasm_closure_pointer, partials,          partial_count};
  Ber make_lval_wasm_lambda_call = BinaryenCall(
      wasm->module, "make_lval_wasm_lambda", operands, 6, make_type_int32(1));

  lambda_list[lambda_block_count++] = make_lval_wasm_lambda_call;

  Ber lambda_block = BinaryenBlock(module, lambda_name, lambda_list,
                                   lambda_block_count, BinaryenTypeAuto());
  free(lambda_list);
  release(lambda_name);
  return cresult(lambda_block);
}

CResult compile_special_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  /* printf("compile special call!!!!!!!!!!!!!!!\n"); */
  char* fn_name = lval_fn_sym->str;
  if (_strcmp(fn_name, "let") == 0) return compile_let(wasm, args);
  if (_strcmp(fn_name, "if") == 0) return compile_if(wasm, args);
  if (_strcmp(fn_name, "do") == 0) {
    Lval* do_list = make_lval_list();
    do_list->head = retain(args);
    CResult result = compile_do_list(wasm, NULL, NULL, do_list);
    release(do_list);
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

CResult compile_sys_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  /* printf("compile sys call!!!!!!!!!!!!!!!\n"); */
  Ber arg_list_head = compile_fn_rest_args(wasm, args).ber;

  Ber lval_list_operands[1] = {arg_list_head};
  Ber wasm_lval_list = BinaryenCall(wasm->module, "new_lval_list",
                                    lval_list_operands, 1, make_type_int32(1));

  Ber sys_fn_operands[2] = {make_int32(wasm->module, 0), wasm_lval_list};
  char* c_fn_name =
      alist_get(wasm->lispy_to_c_fn_map, is_eq_str, lval_fn_sym->str);

  if (!c_fn_name)
    quit(wasm, "System fn not found in runtime: %s", lval_fn_sym->str);

  Ber sys_fn_call = BinaryenCall(wasm->module, c_fn_name, sys_fn_operands, 2,
                                 make_type_int32(1));
  // TODO: after sys call, release list, which will also release all its
  // cells.

  return cresult(sys_fn_call);
}

CResult compile_wasm_fn_call(Wasm* wasm, Ber lval_wasm_ref, char* fn_name,
                             Cell* args) {
  BinaryenModuleRef module = wasm->module;
  Context* context = wasm->context->car;
  /* printf("compile local ref call!!!!!!!!!!!!!!!\n"); */
  // Puts args of a fn like (f a b c) on stack like this: <c b a 3> where
  // stack pointer points at <
  int wasm_args_count = 0;
  int args_count = list_count(args);
  int max_wasm_args_count = args_count + 5;  // args,  update stackpointer,
  // call, reset stackpointer, release_locals, result
  Ber* wasm_args = malloc(sizeof(Ber) * max_wasm_args_count);
  int sp_offset = args_count * 4;
  Ber stack_pointer =
      BinaryenGlobalGet(wasm->module, "stack_pointer", BinaryenTypeInt32());

  LocalIndices* li = li_init();
  // Put args on stack

  printf("compile_wasm_fn_call %s\n", fn_name);
  while (args) {
    // NOTE: setting flags like is bad coding. Ideally we would like to return
    // not a Ber but a struct with data on the compile. We can include in
    // there whether we just compiled a fn call or not, together with the
    // resultant Ber.
    CResult result = lval_compile(wasm, args->car);
    Ber compiled_arg = result.ber;
    Ber push_arg = NULL;
    if (result.is_fn_call) {
      int local_index = li_track(wasm, li, li_new(wasm));
      compiled_arg = BinaryenLocalTee(module, local_index, compiled_arg,
                                      BinaryenTypeInt32());
    } else {
      /* printf("arg: "); */
    }

    /* lval_println(args->car); */
    push_arg =
        BinaryenStore(module, 4, sp_offset - (wasm_args_count + 1) * 4, 0,
                      stack_pointer, compiled_arg, BinaryenTypeInt32());
    wasm_args[wasm_args_count++] = push_arg;
    args = args->cdr;
  }

  // Increase stack pointer to free mem.
  BinaryenExpressionRef sp_plus_offset = BinaryenBinary(
      module, BinaryenAddInt32(), stack_pointer, make_int32(module, sp_offset));
  wasm_args[wasm_args_count++] =
      BinaryenGlobalSet(module, "stack_pointer", sp_plus_offset);

  // Call fn
  Ber result = NULL;
  if (lval_wasm_ref) {
    // TODO: write in wasm:
    // currently we assume lval_wasm_ref === LVAL_WASM_LAMBDA
    // so we call indirect, as below
    // TODO: implement using maps and sets and vectors as fns.
    // else throw runtime error. (let [a 1] (a 2)) , can't use number as fn

    // Get fn_index and closure pointer from lval_wasm_ref
    int fn_table_index_offset = 2;
    Ber wasm_fn_index = BinaryenLoad(module, 2, 0, fn_table_index_offset, 0,
                                     BinaryenTypeInt32(), lval_wasm_ref);
    int closure_pointer_offset = 12;
    Ber wasm_closure_pointer =
        BinaryenLoad(module, 4, 0, closure_pointer_offset, 0,
                     BinaryenTypeInt32(), lval_wasm_ref);

    // Pass closure pointer and args count to wasm fn
    Ber operands[] = {wasm_closure_pointer, make_int32(module, args_count)};
    BinaryenType params_type = make_type_int32(2);

    result = BinaryenCallIndirect(module, wasm_fn_index, operands, 2,
                                  params_type, BinaryenTypeInt32());
  }

  else if (fn_name) {
    printf("BinaryenCall\n");
    Ber operands[] = {make_int32(module, 0), make_int32(module, args_count)};
    result = BinaryenCall(module, fn_name, operands, 2, BinaryenTypeInt32());
  } else
    quit(wasm,
         "Pass either a fn_name or a lval_wasm_ref to compile_wasm_fn_call ");

  // Store result in local wasm var
  int result_index = context->function_context->local_count++;
  wasm_args[wasm_args_count++] = BinaryenLocalSet(module, result_index, result);

  // Reset stack pointer;
  BinaryenExpressionRef sp_minus_offset = BinaryenBinary(
      module, BinaryenSubInt32(), stack_pointer, make_int32(module, sp_offset));
  wasm_args[wasm_args_count++] =
      BinaryenGlobalSet(module, "stack_pointer", sp_minus_offset);

  // Release results of fn calls
  Ber release_locals_block =
      li_release(wasm, li, "release_locals_for_fn_call").ber;
  if (release_locals_block) wasm_args[wasm_args_count++] = release_locals_block;

  // Get result from local wasm var
  Ber result_from_local =
      BinaryenLocalGet(module, result_index, BinaryenTypeInt32());
  wasm_args[wasm_args_count++] = result_from_local;

  Ber call_block =
      BinaryenBlock(module, uniquify_name(wasm, "lambda_call"), wasm_args,
                    wasm_args_count, BinaryenTypeInt32());

  free(wasm_args);
  li_close(li);
  printf("Returning call block\n");
  return cresult(call_block);
}

/* Ber compile_root_fn_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) { */
/*   return compile_wasm_fn_call(wasm, NULL, lval_fn_sym->str, args); */
/* } */

CResult compile_list(Wasm* wasm, Lval* lval_list);

CResult compile_as_fn_call(Wasm* wasm, Lval* lval, Cell* args) {
  switch (lval->type) {
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:;
          Ber compiled_list = compile_list(wasm, lval).ber;
          return compile_wasm_fn_call(wasm, compiled_list, NULL, args);
        case VECTOR:
        case MAP:
        case SET:
          quit(wasm, "ERROR: Map/set/vector cannot be cast as fn (yet)\n");
        default:
          quit(wasm, "ERROR: Unknown collection subtype %d", lval->subtype);
      }
      break;
    case LVAL_LITERAL:
      quit(wasm,
           "ERROR: Literal or symbol cannot be cast as fn (not even a "
           "keyword, "
           "yet)\n");
    case LVAL_ERR:
      quit(wasm, "ERROR compile_list: there was an error in parsing code: %s\n",
           lval->str);
    default:
      quit(wasm, "ERROR compile_list: Unknown lval type %d\n", lval->type);
  }
  return cnull();
}

CResult compile_lval_compiler(Wasm* wasm, Lval* lval_symbol,
                              Lval* lval_compiler);

// A non empty list is taken to be a fn call. We do our best to resolve the expr
// in the list as a fn call, and pass the rest of the list as args.
CResult compile_list(Wasm* wasm, Lval* lval_list) {
  /* BinaryenModuleRef module = wasm->module; */
  CONTEXT_LVAL("compile_list", lval_list);
  lval_println(lval_list);

  // Empty list
  if (lval_list->head == NIL) {
    if (!wasm->lval_empty_list_offset.ber)
      wasm->lval_empty_list_offset = inter_lval(wasm, lval_list);
    return wasm->lval_empty_list_offset;  // pointer to empty lval list
  }

  Lval* lval_first = lval_list->head->car;

  Cell* args = lval_list->head->cdr;
  CResult fn_call = cnull();

  // Fn call > (symbol args ...)
  if (lval_first->type == LVAL_SYMBOL) {
    /* printf("resolving symbol %s\n", lval_first->str); */
    // Let's see if the symbol refers to something we know how to compile
    // already
    Lval* resolved_symbol = lenv_get(wasm->env, lval_first);
    // If it's a symbol it has to be known in our compiler env!!!
    if (resolved_symbol->type == LVAL_ERR) {
      lval_println(lval_list);
      lval_println(lval_first);
      quit(wasm, "ERROR: Unknowm symbol while trying to compile a list: %s",
           lval_first->str);
    }

    switch (resolved_symbol->type) {
      case LVAL_FUNCTION:  // as evalled in our compiler env
        switch (resolved_symbol->subtype) {
          case SYS:
            /* lval_println(resolved_symbol); */
            /* printf("sys fn: %s\n", resolved_symbol->str); */
            fn_call = compile_sys_call(wasm, resolved_symbol, args);
            break;
          case SPECIAL:
            fn_call = compile_special_call(wasm, resolved_symbol, args);
            break;
          case LAMBDA:  // root functions in compiler env
            printf("Calling lambda!! %s\n", lval_first->str);
            fn_call = compile_wasm_fn_call(wasm, NULL, lval_first->str, args);
            break;
            /* return compile_root_fn_call(wasm, lval_first, args); */
          case MACRO:;
            Lval* arg_list = make_lval_list();
            arg_list->head = retain(args);
            Lval* bound_macro = expand_macro(resolved_symbol, arg_list);
            release(arg_list);
            CResult result = lval_compile(wasm, bound_macro);
            release(bound_macro);
            return result;
          default:
            /* lval_println(resolved_symbol); */
            quit(wasm,
                 "ERROR: Can't compile function with unknown subtype %d\n",
                 lval_first->subtype);
        }
        break;
      case LVAL_COMPILER:;
        // we've assigned something to a local ref in wasm, but we don't know
        // what it is, and whether it's a fn even. It'll have to be worked out
        // at runtime. With some clever optimisations we can in some cases
        // probably work out what it is at compile time and can reduce it to a
        // compile_list/lambda_cal. However the default case is that we cannot
        // assume anything about the lval.

        Ber compiled_symbol =
            compile_lval_compiler(wasm, lval_first, resolved_symbol).ber;
        /* Ber compiled_symbol = resolve_symbol_and_compile(wasm,
         * lval_first).ber; */
        fn_call = compile_wasm_fn_call(wasm, compiled_symbol, NULL, args);
        break;
      case LVAL_SYMBOL:
        quit(wasm, "ERROR: A symbol can't be cast to a fn: %s",
             resolved_symbol->str);
      case LVAL_COLLECTION:
        switch (resolved_symbol->subtype) {
          case LIST:
            quit(wasm, "ERROR: A list can't be cast to a fn");
          default:;
        }
      default:;
        // Not a function or wasm ref, perhaps it's something else we can use
        // as a fn
        fn_call = compile_as_fn_call(wasm, resolved_symbol, args);
    }
  } else {
    // Not a function or wasm ref, perhaps it's something else we can use as a
    // fn
    // TODO: make sure is_fn_call is set properly when we use a map as a fn!!!
    fn_call = compile_as_fn_call(wasm, lval_first, args);
  }
  fn_call.is_fn_call = 1;
  return fn_call;
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
  switch (lval->type) {
    case LVAL_SYMBOL:;
      // Resolve symbol in our compiler env and compile it. At runtime there's
      // no notion of environments or symbols that resolve to other lvalues.
      // Symbols at runtime are just that, a literal similar to strings, numbers
      // etc.
      Lval* lval_resolved_sym = lenv_get(wasm->env, lval);
      /* lval_println(lval_resolved_sym); */
      switch (lval_resolved_sym->type) {
        case LVAL_ERR:
          return quit(wasm, lval_resolved_sym->str);
        case LVAL_COMPILER:
          // symbol has been added to env while compiling, so
          // it's a closure var, param or local (let) var
          return compile_lval_compiler(wasm, lval, lval_resolved_sym);
        case LVAL_FUNCTION:  // as evalled in our compiler env
          switch (lval_resolved_sym->subtype) {
            /* case SYS: */
            /*   /\* return datafy_sys_fn(wasm, lval_resolved_sym); *\/ */
            /*   break; */
            case MACRO:
            case LAMBDA:  // functions in compiler env
              if (lval_resolved_sym->str == NULL)
                lval_resolved_sym->str = retain(lval->str);
              /* return compile_global_lambda(wasm, lval->str,
               * lval_resolved_sym); */
              /* case SPECIAL: */
              /*   return quit( */
              /*       wasm, "ERROR: Can't take value of a special form such as
               * %s", */
              /*       lval_resolved_sym->str); */
              /* case MACRO: */
              /*   return quit(wasm, "ERROR: Can't take value of a macro such as
               * %s", */
              /*               lval_resolved_sym->str); */
              /* default:; */
              /*   return quit(wasm, */
              /*               "ERROR: Can't compile function with subtype
               * %d\n", */
              /*               lval_resolved_sym->subtype); */
          }
        default:
          lval = lval_resolved_sym;
          /* return datafy_lval(wasm, lval_resolved_sym); */
      }
      break;
    case LVAL_COLLECTION:
      if (lval->subtype == LIST) return compile_list(wasm, lval);  // fn call
    default:;
  }
  return datafy_lval(wasm, lval);
}

void print_pair(Lval* lval_sym, Lval* lval) {
  lval_print(lval_sym);
  printf(": ");
  lval_print(lval);
}

int compile(char* file_name) {
  /* Lval* lval_array[] = {make_lval_nil(), make_lval_num(123)}; */
  /* Lval* lval_list = make_rest_args(lval_array, 2); */
  /* lval_println(lval_list); */
  /* print_slot_size(); */

  set_log_level(LOG_LEVEL_INFO);

  Wasm* wasm = init_wasm();
  /* printf("__data_end: %d\n", wasm->__data_end); */
  /* wasm->env = interprete_file(file_name); */
  Lenv* user_env = interprete_file(file_name);
  env_print(user_env);

  import_runtime(wasm);

  printf("Processing env ==============================\n");
  Cell* cell = user_env->kv;
  // NOTE: we're choosing to compile all fns here, but alternatively we could
  // just compile one, which would then compile other fns as needed.
  while (cell) {
    Cell* pair = cell->car;
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;

    /* print_pair(lval_sym, lval); */
    if (lval->type == LVAL_FUNCTION && lval->subtype == LAMBDA &&
        lval->offset == -1) {
      add_wasm_function(wasm, lval->closure, lval_sym->str, lval);
      printf("lval_fun offset %d\n", lval->offset);
    }

    cell = cell->cdr;
    /* if (cell) printf("%s", "\n"); */
    /* printf("\n"); */
  }

  BinaryenAddFunctionExport(wasm->module, "test", "test");

  add_fn_to_table(wasm, "log_int");
  add_fn_to_table(wasm, "printf_");
  // We only have this data _after_ compiling
  /* add_global_section(module); */
  add_function_table(wasm);

  /* add_string_to_data(wasm, "foo3"); */
  add_memory_section(wasm);

  char* contents = "hello there!!!";
  BinaryenAddCustomSection(wasm->module, "foo", contents, _strlen(contents));

  int validate_result = BinaryenModuleValidate(wasm->module);
  if (!validate_result)
    printf("VALIDATE ERROR!!!!!!\n");
  else
    printf("VALIDATED OK!!\n");
  /* BinaryenModulePrint(wasm->module); */

  write_wat(wasm, "compiled/lispy.wat");
  write_wasm(wasm, "compiled/lispy.wasm");

  release_env(user_env);
  free_wasm(wasm);

  return 0;
}

/* Lval* parse_file(char* file_name) { */
/*   char* str = read_file(file_name); */
/*   if (!str) { */
/*     printf("Could not load file %s", str); */
/*     return NULL; */
/*   } */

/*   int pos = 0; */
/*   Lval* lval_list = lval_read_list(str, &pos, '\0'); */
/*   if (lval_list->type == LVAL_ERR) { */
/*     lval_println(lval_list); */
/*     return NULL; */
/*   } */
/*   return lval_list; */
/* } */

/* void add_global_section(BinaryenModuleRef module) { */
/*   BinaryenAddGlobalImport(module, "data_end", "env", "data_end", */
/*                           BinaryenTypeInt32(), 0); */ /*   BinaryenAddGlobal(module,
                             "a-global",
                             BinaryenTypeInt32(),
                             0, */
/*                     make_int32(module, 7)); */

/*   BinaryenAddGlobal(module, "a-mutable-global", BinaryenTypeFloat32(), 1,
 */
/*                     make_int32(module, 123)); */
/* } */

/* BER call_indirect_example(Wasm* wasm) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   BER fn_index = make_int32(module, 0); */
/*   BER operands[] = {wasm_offset(wasm, 5), */
/*                                       make_int32(module, 0)}; */

/*   BinaryenType params_type = make_type_int32(2); */

/*   BinaryenType results_type = make_type_int32(0); */

/*   BER call_indirect = BinaryenCallIndirect( */
/*       module, fn_index, operands, 2, params_type, results_type); */

/*   BER drop = BinaryenDrop(module, call_indirect); */
/*   return drop; */
/* } */

/* struct table_fn { */
/*   char* fn_name; */
/*   int table_index; */
/*   int operands_count; */
/*   BinaryenType operands_type; */
/*   int results_count; */
/* }; */

/* add_string_to_data(wasm, "foo3"); */
/* add_string_to_data(wasm, "bar"); */

/* BER compile_symbol_literal(Wasm* wasm, Lval* lval) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   int len = _strlen(lval->str); */
/*   BER wasm_len = make_int32(module, len); */
/*   BER operands_lalloc[1] = {wasm_len}; */
/*   BER lalloc_size = BinaryenCall( */
/*       module, "lalloc_size", operands_lalloc, 1, make_type_int32(1)); */

/*   // TODO: see if str already exists and return that offset instead of
 * adding
 */
/*   // str again to data */
/*   int offset = add_string_to_data(wasm, lval->str); */

/*   BER operands_strcpy[2] = {lalloc_size, */
/*                                               wasm_offset(wasm, offset)};
 */
/*   BER _strcpy = */
/*       BinaryenCall(module, "_strcpy", operands_strcpy, 2,
 * make_type_int32(1)); */
/*   BER operands[1] = {_strcpy}; */

/*   return BinaryenCall(module, "make_lval_sym", operands, 1,
 * make_type_int32(1)); */
/* } */

/* BER compile_lval_literal(Wasm* wasm, Lval* lval) { */
/*   BinaryenModuleRef module = wasm->module; */
/*   /\* printf("lval_compile literal: "); *\/ */
/*   BER* operands = NULL; */
/*   char* func_name; */
/*   int operands_count = 1; */
/*   switch (lval->subtype) { */
/*     case NUMBER:; */
/*     case STRING:; */
/*       return compile_lval_literal(wasm, lval); */
/*       /\* int len = _strlen(lval->str); *\/ */
/*       /\* BER wasm_len = make_int32(module, len); *\/ */
/*       /\* BER operands_lalloc[1] = {wasm_len}; *\/ */
/*       /\* BER lalloc_size = BinaryenCall( *\/ */
/*       /\*     module, "lalloc_size", operands_lalloc, 1,
 * make_type_int32(1));
 * *\/ */

/*       /\* int offset = add_string_to_data(wasm, lval->str); *\/ */

/*       /\* BER operands_strcpy[2] = {lalloc_size, *\/ */
/*       /\*                                             wasm_offset(wasm,
 * offset)}; */
/*        *\/ */
/*       /\* BER _strcpy = BinaryenCall( *\/ */
/*       /\*     module, "_strcpy", operands_strcpy, 2, make_type_int32(1));
 * *\/
 */
/*       /\* BER operands_string[1] = {_strcpy}; *\/ */

/*       /\* operands = operands_string; *\/ */
/*       /\* func_name = "make_lval_str"; *\/ */
/*       /\* break; *\/ */
/*     case LNIL: */
/*       operands_count = 0; */
/*       func_name = "make_lval_nil"; */
/*       break; */
/*     case LTRUE: */
/*       operands_count = 0; */
/*       func_name = "make_lval_true"; */
/*       break; */
/*     case LFALSE: */
/*       operands_count = 0; */
/*       func_name = "make_lval_false"; */
/*       break; */
/*     default: */
/*       return make_int32(module, 123); */
/*   } */

/*   BER literal = BinaryenCall( */
/*       module, func_name, operands, operands_count, make_type_int32(1)); */

/*   return literal; */
/* } */

/* printf("offset: %d\n", wasm->strings_offset); */
/* /\* printf("S2 %s:\n", s2); *\/ */
/* char* s = (char*)data_lval; */

/* for (int i = 0; i < 14; i++) printf("%x,", data_lval[i]); */
/* printf("\n"); */
/* for (int i = 0; i < 36; i++) { */
/*   if (i % 4 == 0) printf("|"); */
/*   printf("%x,", s[i]); */
/* } */

/* BER BinaryenCall(BinaryenModuleRef module, */
/*                                    const char* target, */
/*                                    BER* operands, */
/*                                    BinaryenIndex numOperands, */
/*                                    BinaryenType returnType); */

/* BINARYEN_API BER */
/* BinaryenCallIndirect(BinaryenModuleRef module, */
/*                      BER target, */
/*                      BER* operands, */
/*                      BinaryenIndex numOperands, */
/*                      BinaryenType params, */
/*                      BinaryenType results); */

/* int __data_end; */

/* compile_fn_rest_args(wasm, lval->body->head); */
/* BER drop = call_indirect_example(wasm); */

/* BER lval_list_operands[1] = {arg_list_head}; */
/* BER lval_list = BinaryenCall( */
/*     module, "new_lval_list", lval_list_operands, 1, make_type_int32(1));
 */

/* BER lval_println_operands[1] = {lval_list}; */
/* BER lval_println_operands[1] = {wasm_offset(wasm, 22)};
 */
/* BER body = BinaryenCall( */
/*     module, "lval_println", lval_println_operands, 1,
 * BinaryenTypeNone());
 */

/* BER print_slot_size = */
/*     BinaryenCall(module, "print_slot_size", NULL, 0, BinaryenTypeNone());
 */
/* body = print_slot_size; */

/* printf("fn-name: %s\n", lval_sym->str); */
/* printf("params: "); */
/* lval_println(lval->params); */
/* printf("body: "); */
/* lval_println(lval->body); */

// Load: align can be 0, in which case it will be the natural alignment (equal
// to bytes)
/* BINARYEN_API BinaryenExpressionRef BinaryenLoad(BinaryenModuleRef module,
 */
/*                                                 uint32_t bytes, */
/*                                                 int8_t signed_, */
/*                                                 uint32_t offset, */
/*                                                 uint32_t align, */
/*                                                 BinaryenType type, */
/*                                                 BinaryenExpressionRef ptr);
 */
/* // Store: align can be 0, in which case it will be the natural alignment
 * (equal */
/* // to bytes) */
/* BINARYEN_API BinaryenExpressionRef BinaryenStore(BinaryenModuleRef module,
 */
/*                                                  uint32_t bytes, */
/*                                                  uint32_t offset, */
/*                                                  uint32_t align, */
/*                                                  BinaryenExpressionRef ptr,
 */
/*                                                  BinaryenExpressionRef
 * value,
 */
/*                                                  BinaryenType type); */

/* void add_wasm_function(Wasm* wasm, char* fn_name, int params_count, */
/*                        int locals_count, Ber body, int results_count) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   BinaryenType params_type = make_type_int32(params_count); */
/*   BinaryenType results_type = make_type_int32(results_count); */
/*   /\* BinaryenType results_type = make_results_type(results_count); *\/ */

/*   BinaryenType* local_types = make_type_int32_array(locals_count); */

/*   BinaryenAddFunction(module, fn_name, params_type, results_type,
 * local_types, */
/*                       locals_count, body); */
/* } */

/* TODO: */
//
// call root fn
// check whether wasm ref is a a lambda/set/map/vector, print error msg if not
// datafy sys and root fns

/* Ber wval_operands[] = {lval_wasm_ref}; */
/* Ber wval_print = BinaryenCall(module, "wval_print", wval_operands, 1, */
/*                               BinaryenTypeNone()); */
/* wasm_args[i++] = wval_print; */

/* BinaryenOp ge = BinaryenGeUInt32(); */
/* Ber ifge = BinaryenBinary(module, ge, args_count, wasm_param_count); */
/* Ber if = BinaryenIf(module, condition, if_true, if_false); */

/* BinaryenModuleRef module = wasm->module; */

/* printf("fn-name: %s\n", lval_sym->str); */
/* printf("params: "); */
/* lval_println(lval_fun->params); */
/* printf("body: "); */
/* lval_println(lval_fun->body); */

// Write wasm that does the following:
// Compile the args to wasm, put them on lispy stack in reverse order, with
// count of args on top slot.

// Check whether the local ref is a lambda (which might be a wrapper for sys
// and named fns)
//
// Write fns to deal with keyword, vector, set and map, dispatch by type and
// pass first node plus args to fn

// It's the callee's responsibility to check correct number of args (so
// could be disabled by flag). Without flag it's possible to call a fn with
// too few args, which would have unpredictable results. Too many doesn't
// hurt necessarily. It's also callee's responsibility to wrap up excess
// args into a rest arg. Otherwise we would have to check for this for every
// fn call. Throw error if count of args < param count (without any rest
// arg): [a b] has param count of 2, but [a b & c] has param count of 2 as
// well. If fn has rest arg then insert code that gathers excess args up
// into list and stores that list in a local var to be referred to in the
// rest of the fn. Only insert param checking code when lispy compiler flag
// is set for it. Same for rest arg, only insert wasm code for it when
// appropriate.

// When a LVAL_WASM_LAMBDA is a result of a partial its fn_table_index
// points to a wrapper fn that puts the partial args after the current stack
// pointer and increases sp by partial_count, and then calls the actual fn
// (which again might be a partial).

// Call indirect fn with the right offset, and one arg (pointer to closure)
// Set stack pointer to free mem; (so add param_count + 1  to sp, last slot
// holds param_count itself).
// On return from this call set stack pointer back.

// Resolve symbol in our compiler env and compile it. At runtime there's no
// notion of environments or symbols that resolve to other lvalues. Symbols at
// runtime are just that, a literal similar to strings, numbers etc
/* CResult resolve_symbol_and_compile(Wasm* wasm, Lval* lval_sym) { */
/*   /\* printf(">>>>>>>>>>>>\n"); *\/ */
/*   /\* printf("COMPILING SYMBOL!!! %s\n", lval_sym->str); *\/ */
/*   Lval* lval_resolved_sym = lenv_get(wasm->env, lval_sym); */
/*   /\* lval_println(lval_resolved_sym); *\/ */
/*   switch (lval_resolved_sym->type) { */
/*     case LVAL_ERR: */
/*       return quit(wasm, lval_resolved_sym->str); */
/*     case LVAL_COMPILER:  // symbol has been added to env while compiling, so
 */
/*       // it's a closure var, param or local (let) var */
/*       return compile_lval_compiler(wasm, lval_sym, lval_resolved_sym); */
/*     case LVAL_FUNCTION:  // as evalled in our compiler env */
/*       switch (lval_resolved_sym->subtype) { */
/*         case SYS: */
/*           return datafy_sys_fn(wasm, lval_resolved_sym); */
/*         case LAMBDA:  // functions in compiler env */
/*           return compile_global_lambda(wasm, lval_sym->str,
 * lval_resolved_sym); */
/*         case SPECIAL: */
/*           return quit(wasm, */
/*                       "ERROR: Can't take value of a special form such as %s",
 */
/*                       lval_resolved_sym->str); */
/*         case MACRO: */
/*           return quit(wasm, "ERROR: Can't take value of a macro such as %s",
 */
/*                       lval_resolved_sym->str); */
/*         default:; */
/*           return quit(wasm, "ERROR: Can't compile function with subtype
 * %d\n", */
/*                       lval_resolved_sym->subtype); */
/*       } */
/*       break; */
/*     default:  // as evalled in our compiler env */
/*       return datafy_lval(wasm, lval_resolved_sym); */
/*       /\* case LVAL_COLLECTION: *\/ */
/*       /\*   return datafy_collection(wasm, lval_resolved_sym); *\/ */
/*       /\* case LVAL_SYMBOL: *\/ */
/*       /\* case LVAL_LITERAL: *\/ */
/*       /\*   return datafy_literal(wasm, lval_resolved_sym); *\/ */
/*   } */

/*   /\* printf("resolved sym: \n"); *\/ */
/*   /\* lval_println(lval_resolved_sym); *\/ */
/* } */
