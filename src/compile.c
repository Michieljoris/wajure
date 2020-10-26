

#include <binaryen-c.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "builtin.h"
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
#include "make_wasm_fn.h"
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

CResult compile_do_list(Wasm* wasm, Lval* lval_list, Ber init) {
  /* printf("COMPILE_DO_LIST!!!!\n"); */
  /* lval_println(lval_list); */
  BinaryenModuleRef module = wasm->module;
  int do_list_count =
      list_count(lval_list->data.head) + 2;  // room for extra bers

  LocalIndices* li = li_init();

  Ber* do_body = malloc(do_list_count * sizeof(Ber));

  int do_list_index = 0;
  if (init) do_body[do_list_index++] = init;

  Ber do_result = NULL;

  Cell* list = lval_list->data.head;
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
CResult compile_lval_ref(Wasm* wasm, char* symbol_str, Lval* lval_ref) {
  /* Lval* lval_wasm_ref = lval_resolved_sym; */
  /* printf( */
  /*     "We've got a LVAL_REF (param, local or closed over binding)!!! "
   */
  /*     "%s\n", */
  /*     lval_symbol->data.str); */
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
  } else {  // Not local, so it's a ref to a closed over value, so it's a param
            // or local from a lexically parent fn

    // We keep track of all symbols that are not local (param or let local) we
    // encounter while compiling a fn in the function_context->symbol_to_ref
    // alist. This is so 1. if we've seen it before we don't add it again, 2.
    // when making the fn object for the current fn we can add a closure block
    // using this alist.
    int closure_index = alist_index_of(context->function_context->symbol_to_ref,
                                       is_eq_str, symbol_str);

    if (closure_index == -1) {
      // Let's add the symbol to the the current function context symbol_to_ref
      // alist, and associate it with a lval_ref. This in effect sends a message
      // to the parent fn context to again try to resolve these lval_refs and
      // attach them to the fn object for the current fn in its closure block.

      closure_index = list_count(context->function_context->symbol_to_ref);
      context->function_context->symbol_to_ref =
          alist_prepend(context->function_context->symbol_to_ref,
                        retain(symbol_str), retain(lval_ref));
    }

    // Retrieve closed over value at closure_index in closure block
    int closure_param = 0;  // first param to fn is closure pointer
    Ber closure_pointer = local_get_int32(wasm->module, closure_param);
    Ber load_closure_lval =
        BinaryenLoad(wasm->module, 4, 0, closure_index * 4, 2,
                     BinaryenTypeInt32(), closure_pointer);

    return cresult(load_closure_lval);
  }
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
  printf("compile local lambda!!!\n");
  BinaryenModuleRef module = wasm->module;

  Context* context = wasm->context->car;

  // Eval the lambda form to get info on params and body for every arity
  Lval* arg_list = new_lval_list(retain(args));
  Lval* lval_fn = eval_lambda(wasm->env, arg_list);
  /* lval_println(lval_fn); */
  release(arg_list);
  if (lval_fn->type == LVAL_ERR) quit(wasm, "Error evalling lambda form");

  char* lambda_name = number_fn_name(wasm, context->function_context->fn_name);
  lval_fn->data.str = lambda_name;
  FunctionData function_data = add_local_lambda(wasm, lval_fn);

  // Which gives us info on the wasm fn's fn_table_index and which and
  // how many bindings it closes over
  int fn_table_index = function_data.fn_table_index;
  Cell* symbol_to_ref = function_data.symbol_to_ref;

  /* printf("COMPILED FUNCTION!!! %d %d %d\n", rest_arg_index, */
  /*        closure_count, param_count); */

  // Make ber versions of our info on the lambda we now have

  // Fn table index
  Ber ber_fn_table_index =
      make_int32(module, wasm->__fn_table_end + fn_table_index);
  if (wasm->pic) {
    Ber fn_table_offset =
        BinaryenGlobalGet(wasm->module, "fn_table_offset", BinaryenTypeInt32());
    ber_fn_table_index = BinaryenBinary(wasm->module, BinaryenAddInt32(),
                                        fn_table_offset, ber_fn_table_index);
  }

  // Partials
  Ber ber_partial_count = make_int32(module, 0);
  Ber ber_partials = make_int32(module, 0);  // NULL

  // Closure
  int symbol_count = list_count(symbol_to_ref);
  Ber* lambda_block_children = malloc((symbol_count + 2) * sizeof(Ber));
  int lambda_block_count = 0;

  // Allocate a block of memory to hold the closure for the lambda, and store
  // pointer to it in local var
  Ber ber_closure_ptr = wasm_lalloc_size(wasm, symbol_count * 4).ber;
  int closure_ptr_local = li_new(wasm);
  Ber local_set = BinaryenLocalSet(module, closure_ptr_local, ber_closure_ptr);
  lambda_block_children[lambda_block_count++] = local_set;

  // Store lval pointers in the closure memory block
  int offset = symbol_count * 4;

  // Resolve collected closure refs so we can attach them to the fn object
  // (lval_wasm_lambda). So these refs (let local or param) we couldn't resolve
  // in the fn we just compiled. We're gonna try to resolve them in the current
  // function context. Some of them again might not be local but again from a
  // parent fn. If so, we add it to the current fn's closure map for tracking
  // it, and load it from the current fn closure block.
  while (symbol_to_ref) {
    Cell* pair = symbol_to_ref->car;
    char* sym = (char*)((Cell*)pair)->car;
    Lval* lval_ref = (Lval*)((Cell*)pair)->cdr;
    Ber closure_lval_pointer = compile_lval_ref(wasm, sym, lval_ref).ber;
    offset -= 4;
    Ber ber_closure_ptr =
        BinaryenLocalGet(module, closure_ptr_local, BinaryenTypeInt32());
    Ber store_wasm_ref =
        BinaryenStore(module, 4, offset, 0, ber_closure_ptr,
                      closure_lval_pointer, BinaryenTypeInt32());
    lambda_block_children[lambda_block_count++] = store_wasm_ref;
    symbol_to_ref = symbol_to_ref->cdr;
  }

  // With this closure ptr we have now all the operands for our
  // make_lval_lambda_call
  ber_closure_ptr =
      BinaryenLocalGet(module, closure_ptr_local, BinaryenTypeInt32());

  // Make a lval_wasm_lambda with info on fn table index, param count, pointer
  // to closure etc.
  Ber operands[4] = {ber_fn_table_index, ber_closure_ptr, ber_partials,
                     ber_partial_count};
  Ber make_lval_wasm_lambda_call = BinaryenCall(
      wasm->module, "make_lval_wasm_lambda", operands, 4, make_type_int32(1));

  // Add this call to the lambda block
  lambda_block_children[lambda_block_count++] = make_lval_wasm_lambda_call;

  // Make and return the block
  Ber lambda_block = BinaryenBlock(module, uniquify_name(wasm, lambda_name),
                                   lambda_block_children, lambda_block_count,
                                   BinaryenTypeInt32());
  free(lambda_block_children);
  release(lambda_name);
  release(symbol_to_ref);
  release(lval_fn);

  CResult ret = {.ber = lambda_block, .is_fn_call = 1};
  return ret;
}

CResult compile_special_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  /* printf("compile special call!!!!!!!!!!!!!!!\n"); */
  char* fn_name = lval_fn_sym->data.str;
  if (_strcmp(fn_name, "let") == 0) return compile_let(wasm, args);
  if (_strcmp(fn_name, "if") == 0) return compile_if(wasm, args);
  if (_strcmp(fn_name, "do") == 0) {
    Lval* do_list = make_lval_list();
    do_list->data.head = retain(args);
    CResult result = compile_do_list(wasm, do_list, NULL);
    release(do_list);
    result.is_fn_call = 1;
    return result;
  }

  if (_strcmp(fn_name, "fn*") == 0) return compile_local_lambda(wasm, args);
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

CResult compile_native_call(Wasm* wasm, Lval* lval_fn_native, Cell* args) {
  WasmFn* builtin_fn = alist_get(state->wajure_to_native_fn_map, is_eq_str,
                                 lval_fn_native->data.str);
  if (!builtin_fn || !builtin_fn->compile_builtin_call)
    quit(wasm, "Function %s not found in runtime", lval_fn_native->data.str);
  return builtin_fn->compile_builtin_call(wasm, *builtin_fn, args);
}

CResult compile_sys_call(Wasm* wasm, Lval* lval_fn_sys, Cell* args) {
  CFn* c_fn =
      alist_get(state->wajure_to_c_fn_map, is_eq_str, lval_fn_sys->data.str);
  if (!c_fn) return compile_native_call(wasm, lval_fn_sys, args);
  char* c_fn_name = c_fn->c_fn_name;

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
  Ber arg_list_head = compile_list(wasm, lval->data.head).ber;

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
  switch (lval->type) {
    case LVAL_SYMBOL:;
      // Resolve symbol in our compiler env and compile it. At runtime there's
      // no notion of environments or symbols that resolve to other lvalues.
      // Symbols at runtime are just that, a literal similar to strings,
      // numbers etc.
      Lval* lval_sym = lval;
      Lval* resolved_sym = eval_symbol(wasm->env, lval_sym);

      switch (resolved_sym->type) {
        case LVAL_ERR:
          return quit(wasm, resolved_sym->data.str);
        case LVAL_REF:;
          // Symbol has been added to env while compiling, so it's a param or
          // local (let) var, either from the current function, or a closed over
          // value.
          CResult ret =
              compile_lval_ref(wasm, lval_sym->data.str, resolved_sym);
          ret.lval = resolved_sym;
          release(resolved_sym);
          return ret;
        case LVAL_FUNCTION:  // as evalled in our compiler env
          switch (resolved_sym->subtype) {
            case MACRO:
            case LAMBDA:  // functions in compiler env
              if (resolved_sym->data.str == NULL)
                resolved_sym->data.str = retain(lval_sym->data.str);
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

  CResult ret = datafy_lval(wasm, lval);
  ret.lval = lval;
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
  set_log_level(LOG_LEVEL_INFO);

  /* printf("FILE_NAME: %s\n", file_name); */
  Wasm* wasm = init_wasm();

  set_current_ns(ns);
  Lenv* env = ns->env;

  /* env_print(env); */

  import_c_fns(wasm);

  printf("Processing env =============\n");
  Cell* head = env->kv;
  int count = list_count(head);
  int i = count - 1;
  Cell** pairs = malloc(count * sizeof(Cell));
  while (head) {
    pairs[i--] = head->car;
    head = head->cdr;
  }

  for (int i = 0; i < count; i++) {
    Cell* pair = pairs[i];
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;

    printf("----Processing: ");
    lval_print(lval_sym);
    printf(": ");
    lval_println(lval);
    if (lval->type == LVAL_FUNCTION) {
      if (lval->subtype == LAMBDA) {  // not interested in compiling macros!
        lval_compile(wasm, lval);
        if (!lval->cfn) add_to_symbol_table(wasm, lval->cname, lval);
      }
    } else {
      datafy_lval(wasm, lval);
      add_to_symbol_table(wasm, lval_sym->data.str, lval);
    }
  }

  printf("===== DONE Processing env\n");

  add_function_table(wasm);

  if (_strcmp(ns->namespace, config->main) == 0) {
    char* main_fn_name = lalloc_size(_strlen(config->main) + 1);
    sprintf(main_fn_name, "%s", config->main);
    Lval* lval_sym = make_lval_sym(main_fn_name);
    Lval* main_fn = lenv_get(ns->env, lval_sym);
    if (!main_fn) quit(wasm, "No main fn found in %s", config->main);
    BinaryenAddFunctionExport(wasm->module, main_fn->cname, "main");
    release(main_fn);
    release(lval_sym);
  }
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
  char* file_name = ns_to_wat(ns->namespace);
  write_wat(wasm, file_name);
  release(file_name);
  file_name = ns_to_wasm(ns->namespace);
  write_wasm(wasm, file_name);
  release(file_name);

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
/* (block $switch$1$leave */
/*        (block $switch$1$default */
/*               (block $switch$1$case$3 */
/*                      (block $switch$1$case$2 */
/*                             (br_table $switch$1$default $switch$1$default
 * $switch$1$case$2 $switch$1$default $switch$1$case$3 $switch$1$case$2
 * $switch$1$default */
/*                                       (i32.const -99))) */
/*                      (i32.const 1) */
/*                      (br $switch$1$leave)) */
/*               (drop */
/*                (i32.const 55)) */
/*               (br $switch$1$leave)) */
/*        (i32.const 3) */
/*        (br $switch$1$leave)) */
