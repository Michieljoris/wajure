#include "compile_fn_call.h"

#include "compile.h"
#include "datafy.h"
#include "eval.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"
#include "print.h"
#include "refcount.h"
#include "wasm.h"
#include "wasm_util.h"

void add_call_fn(Wasm* wasm, int n) {
  char* fn_name = number_fn_name(wasm, "call");  // call#0 - call#n
  add_fn_to_table(wasm, fn_name);
  BinaryenType fn_params_type =
      make_type_int32(2);  // closure pointer and fn_table_index
  BinaryenType fn_results_type = make_type_int32(1);
  BinaryenType call_params_type =
      make_type_int32(n + 1);  // first param is closure_pointer
  BinaryenType call_results_type = make_type_int32(1);

  Ber stack_pointer =
      BinaryenGlobalGet(wasm->module, "stack_pointer", BinaryenTypeInt32());
  Ber closure_pointer = BinaryenLocalGet(wasm->module, 0, BinaryenTypeInt32());
  Ber* operands = malloc((n + 1) * sizeof(Ber));
  operands[0] = BinaryenLoad(wasm->module, 4, 0, 0, 2, BinaryenTypeInt32(),
                             closure_pointer);

  for (int i = 0; i < n; i++) {
    operands[i + 1] = BinaryenLoad(wasm->module, 4, 0, i * 4, 2,
                                   BinaryenTypeInt32(), stack_pointer);
  }
  Ber wval_fn_table_index =
      BinaryenLocalGet(wasm->module, 1, BinaryenTypeInt32());
  Ber body = BinaryenCallIndirect(wasm->module, wval_fn_table_index, operands,
                                  n + 1, call_params_type, call_results_type);

  BinaryenAddFunction(wasm->module, fn_name, fn_params_type, fn_results_type,
                      NULL, 0, body);
  free(operands);
  release(fn_name);
}

#define MAX_FN_PARAMS 20

// This ads MAX_FN_PARAMS wasm fns (call#0-call#20), each fn takes 2 params:
// closure_pointer and
void add_call_fns(Wasm* wasm) {
  for (int i = 0; i <= MAX_FN_PARAMS; i++) add_call_fn(wasm, i);
}

Ber compile_args_into_block(Wasm* wasm, Cell* args, LocalIndices* li) {
  BinaryenModuleRef module = wasm->module;
  /* Ber wval_type = get_wval_prop(module, wval, "type"); */

  int args_count = list_count(args);

  Ber stack_pointer =
      BinaryenGlobalGet(wasm->module, "stack_pointer", BinaryenTypeInt32());
  Ber* children = malloc(args_count * sizeof(Ber));

  int i = 0;
  // Put args on stack ====================================================
  while (args) {
    CResult result = lval_compile(wasm, args->car);
    Ber compiled_arg = result.ber;
    if (result.is_fn_call) {
      int local_index = li_track(wasm, li, li_new(wasm));
      compiled_arg = BinaryenLocalTee(module, local_index, compiled_arg,
                                      BinaryenTypeInt32());
    }

    int offset = i * 4;
    Ber push_arg = BinaryenStore(module, 4, offset, 2, stack_pointer,
                                 compiled_arg, BinaryenTypeInt32());
    children[i++] = push_arg;
    args = args->cdr;
  }

  Ber block = BinaryenBlock(module, uniquify_name(wasm, "args"), children,
                            args_count, BinaryenTypeNone());
  free(children);
  return block;
}

Ber get_wval_prop(BinaryenModuleRef module, Ber wval, char* prop) {
  Ber operands[] = {wval};
  char* fn_call_prefix = "get_wval_";
  char* fn_call = malloc(_strlen(fn_call_prefix) + _strlen(prop) + 1);
  sprintf(fn_call, "%s%s", fn_call_prefix, prop);
  printf("wval get call: %s\n", fn_call);
  Ber ret = BinaryenCall(module, fn_call, operands, 1, make_type_int32(1));
  free(fn_call);
  return ret;
}

Ber validate_as_fn(Wasm* wasm, Ber wval) {
  BinaryenModuleRef module = wasm->module;
  Ber wval_type = get_wval_prop(module, wval, "type");
  Ber is_not_wval_fun = BinaryenBinary(wasm->module, BinaryenNeInt32(),
                                       wval_type, make_int32(module, WVAL_FUN));
  char* rt_error_msg = "not a fn";
  scoped char* str = lalloc_size(_strlen(rt_error_msg));
  Ber not_a_fn_rt_error = wasm_runtime_error(wasm, RT_NOT_A_FN, str).ber;
  Ber all_ok = BinaryenNop(module);
  return BinaryenIf(module, is_not_wval_fun, not_a_fn_rt_error, all_ok);
}

Ber call_bundle_rest_args(Wasm* wasm, Ber args_block_start, Ber args_count,
                          Ber wval_param_count, Ber wval_rest_arg_index) {
  BinaryenModuleRef module = wasm->module;
  Ber offset = BinaryenBinary(module, BinaryenMulInt32(), make_int32(module, 4),
                              wval_rest_arg_index);  // TODO: -1!!!!

  Ber rest_args_start =
      BinaryenBinary(module, BinaryenAddInt32(), args_block_start, offset);

  Ber rest_arg_length = BinaryenBinary(module, BinaryenSubInt32(), args_count,
                                       wval_rest_arg_index);
  Ber operands[] = {rest_args_start, rest_arg_length};
  return BinaryenCall(module, "bundle_rest_args", operands, 2,
                      BinaryenTypeNone());
}

Ber call_fn_by_ref(Wasm* wasm, Ber wval, Cell* args, Ber* block_children,
                   int* block_children_count, LocalIndices* li) {
  BinaryenModuleRef module = wasm->module;
  Ber wval_fn_index = get_wval_prop(module, wval, "fn_table_index");
  Ber wval_param_count = get_wval_prop(module, wval, "param_count");
  Ber wval_closure = get_wval_prop(module, wval, "closure");
  /* Ber wval_rest_arg_index = get_wval_prop(module, wval, "rest_arg_index"); */

  block_children[(*block_children_count)++] = validate_as_fn(wasm, wval);

  Ber args_block = compile_args_into_block(wasm, args, li);
  block_children[(*block_children_count)++] = args_block;

  // Pass closure pointer and fn_table_index count to wasm call fn
  Ber call_operands[] = {wval_closure, wval_fn_index};

  // The first 20 fns in the global fn table redirect to the real fn with
  // index wval_fn_index, loading the right number of args from the args
  // block.
  Ber fn_table_index = wval_param_count;
  return BinaryenCallIndirect(module, fn_table_index, call_operands, 2,
                              make_type_int32(2), BinaryenTypeInt32());
}

Ber* compile_args_into_operands(Wasm* wasm, char* fn_name, Lval* lval_fun,
                                Cell* args, LocalIndices* li) {
  BinaryenModuleRef module = wasm->module;
  int param_count = lval_fun->param_count;
  int min_param_count = param_count - 1;
  int args_count = list_count(args);
  int has_rest_arg = lval_fun->rest_arg_index;

  if (has_rest_arg) {
    if (args_count < min_param_count)
      quit(wasm, "Not enough args passed to %s (need at least %d)", fn_name,
           param_count - 1);
  } else {
    if (args_count != param_count)
      quit(wasm, "Wrong number of args (%d) passed to %s (%d expected)",
           args_count, fn_name, param_count);
  }

  Ber* call_operands = malloc((1 + param_count) * sizeof(Ber));
  Cell* head = args;
  int i = 0;
  Ber closure_pointer = make_int32(module, 0);
  call_operands[i++] = closure_pointer;
  for (int j = 0; j < min_param_count; j++) {
    /* while (head) { */
    CResult result = lval_compile(wasm, head->car);
    Ber compiled_arg = result.ber;
    if (result.is_fn_call) {
      int local_index = li_track(wasm, li, li_new(wasm));
      compiled_arg = BinaryenLocalTee(module, local_index, compiled_arg,
                                      BinaryenTypeInt32());
    }

    call_operands[i++] = compiled_arg;
    head = head->cdr;
  }

  if (has_rest_arg) {
    if (head) {
      Ber list = compile_list(wasm, head).ber;

      Ber lval_list_operands[1] = {list};
      int wasm_lval_list_index = li_track(wasm, li, li_new(wasm));
      Ber wasm_lval_list = BinaryenCall(
          module, "new_lval_list", lval_list_operands, 1, make_type_int32(1));
      wasm_lval_list = BinaryenLocalTee(module, wasm_lval_list_index,
                                        wasm_lval_list, BinaryenTypeInt32());
      call_operands[i++] = wasm_lval_list;
    } else
      call_operands[i++] = datafy_nil(wasm).ber;
  }

  return call_operands;
}

Ber call_fn_by_name(Wasm* wasm, char* fn_name, Lval* lval_fun, Cell* args,
                    LocalIndices* li) {
  Ber* call_operands =
      compile_args_into_operands(wasm, fn_name, lval_fun, args, li);

  /* printf("application %d %li\n", lval_fun->param_count, */
  /*        sizeof(call_operands) / sizeof(Ber)); */
  int param_count = lval_fun->param_count + 1;
  Ber result = BinaryenCall(wasm->module, fn_name, call_operands, param_count,
                            BinaryenTypeInt32());
  free(call_operands);
  return result;
}

Ber call_global_fn(Wasm* wasm, Ber fn_table_index, Lval* lval_fun, Cell* args,
                   LocalIndices* li) {
  Ber* call_operands =
      compile_args_into_operands(wasm, lval_fun->str, lval_fun, args, li);
  int param_count = lval_fun->param_count + 1;
  Ber result = BinaryenCallIndirect(wasm->module, fn_table_index, call_operands,
                                    param_count, make_type_int32(param_count),
                                    BinaryenTypeInt32());
  free(call_operands);
  return result;
}

CResult apply(Wasm* wasm, int fn_ref_type, union FnRef fn_ref, Lval* lval_fun,
              Cell* args) {
  BinaryenModuleRef module = wasm->module;
  Context* context = wasm->context->car;
  /* printf("compile local ref call!!!!!!!!!!!!!!!\n"); */
  /* printf("compile_wasm_fn_call %s\n", fn_name); */

  int block_children_count = 0;

  // put partials on stack, args, release_locals, result
  /* int block_size = 6; */  //?????????????????
  int block_size = 99;       // TODO: for debugging
  int max_block_children_count = list_count(args) + block_size;
  Ber* block_children = malloc(sizeof(Ber) * max_block_children_count);

  LocalIndices* li = li_init();

  // Call fn ================================================================
  Ber result = NULL;
  switch (fn_ref_type) {
    case BER: {
      Ber wval = fn_ref.ber;
      // TODO: implement using maps and sets and vectors and keywords as fns.
      /* Ber get_type_call = get_wval_prop(module, wval, "type"); */
      /* block_children[block_children_count++] = */
      /*     wasm_log_int(wasm, get_type_call); */
      result = call_fn_by_ref(wasm, wval, args, block_children,
                              &block_children_count, li);

    } break;
    case FN_NAME: {
      result = call_fn_by_name(wasm, fn_ref.fn_name, lval_fun, args, li);
    } break;
    case GLOBAL: {
      char* fn_table_index_global = fn_ref.global_name;
      Ber wasm_fn_index = BinaryenGlobalGet(wasm->module, fn_table_index_global,
                                            BinaryenTypeInt32());
      result = call_global_fn(wasm, wasm_fn_index, lval_fun, args, li);
    } break;
    default:
      quit(wasm, "Unknown fn_ref_type %d", fn_ref_type);
  }

  // Store fn call result in local wasm var
  // =========================================
  int result_index = context->function_context->local_count++;
  block_children[block_children_count++] =
      BinaryenLocalSet(module, result_index, result);

  // Release results of fn calls =============================================
  Ber release_locals_block =
      li_release(wasm, li, "release_locals_for_fn_call").ber;
  if (release_locals_block)
    block_children[block_children_count++] = release_locals_block;

  // Get result from local wasm var =========================================
  Ber result_from_local =
      BinaryenLocalGet(module, result_index, BinaryenTypeInt32());
  block_children[block_children_count++] = result_from_local;

  // Call block ============================================================
  Ber call_block =
      BinaryenBlock(module, uniquify_name(wasm, "lambda_call"), block_children,
                    block_children_count, BinaryenTypeInt32());

  free(block_children);
  li_close(li);
  /* printf("Returning call block\n"); */
  return cresult(call_block);
}

CResult compile_as_fn_call(Wasm* wasm, Lval* lval, Cell* args) {
  switch (lval->type) {
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:;
          Ber compiled_list = compile_application(wasm, lval).ber;
          int anon_lambda_index = li_new(wasm);
          Ber local_set_anon_lambda =
              BinaryenLocalSet(wasm->module, anon_lambda_index, compiled_list);
          Ber local_get_anon_lambda = BinaryenLocalGet(
              wasm->module, anon_lambda_index, BinaryenTypeInt32());
          int fn_call_result_index = li_new(wasm);
          union FnRef fn_ref = {.ber = local_get_anon_lambda};
          Ber fn_call = apply(wasm, BER, fn_ref, NULL, args).ber;
          Ber local_set_fn_result =
              BinaryenLocalSet(wasm->module, fn_call_result_index, fn_call);
          Ber local_get_fn_result = BinaryenLocalGet(
              wasm->module, fn_call_result_index, BinaryenTypeInt32());
          Ber release_anon_lambda =
              wasm_release(wasm, local_get_anon_lambda).ber;
          Ber children[] = {local_set_anon_lambda, local_set_fn_result,
                            release_anon_lambda, local_get_fn_result};

          Ber block =
              BinaryenBlock(wasm->module, uniquify_name(wasm, "anon-lambda"),
                            children, 4, BinaryenTypeInt32());
          return cresult(block);
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

// A non empty list is taken to be a fn call. We do our best to resolve the
// expr in the list as a fn call, and pass the rest of the list as args.
CResult compile_application(Wasm* wasm, Lval* lval_list) {
  /* BinaryenModuleRef module = wasm->module; */
  CONTEXT_LVAL("apply", lval_list);
  /* lval_println(lval_list); */

  // Empty list
  if (lval_list->head == NIL) return datafy_empty_list(wasm, lval_list);

  Lval* lval_first = lval_list->head->car;
  Cell* args = lval_list->head->cdr;
  CResult fn_call = cnull();

  // Fn call > (symbol args ...)
  if (lval_first->type == LVAL_SYMBOL) {
    // Let's see if the symbol refers to something we know how to compile
    // already

    struct resolved_symbol result = eval_symbol(wasm->env, lval_first);
    Lval* resolved_symbol = result.lval;
    int symbol_is_external = result.ns != NULL;

    // If it's a symbol it has to be known in our compiler env!!!
    if (resolved_symbol->type == LVAL_ERR) {
      lval_println(lval_list);
      lval_println(lval_first);
      quit(wasm, "ERROR: Can't apply an unknowm symbol: %s", lval_first->str);
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
            /* printf("Calling lambda!! %s\n", lval_first->str); */
            printf("APPLY LAMBDA: ");
            lval_println(resolved_symbol);
            if (symbol_is_external) {
              char* global_name =
                  make_global_name("fn:", result.ns->namespace, result.name);
              add_dep(wasm, global_name);
              union FnRef fn_ref = {.global_name = global_name};
              fn_call = apply(wasm, GLOBAL, fn_ref, resolved_symbol, args);
            } else {
              union FnRef fn_ref = {.fn_name = lval_first->str};
              fn_call = apply(wasm, FN_NAME, fn_ref, resolved_symbol, args);
            }
            break;
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
      case LVAL_REF:;
        // we've assigned something to a local or closed over ref in wasm, but
        // we don't know what it is, and whether it's a fn even. It'll have to
        // be worked out at runtime. With some clever optimisations we can in
        // some cases probably work out what it is at compile time and can
        // reduce it to a compile_list/lambda_call. However the default case is
        // that we cannot assume anything about the lval.

        Ber compiled_symbol =
            compile_lval_ref(wasm, lval_first, resolved_symbol).ber;

        union FnRef fn_ref = {.ber = compiled_symbol};
        fn_call = apply(wasm, BER, fn_ref, NULL, args);
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
        // Fall through.
      default:;
        // Not a function or wasm ref, perhaps it's something else we can use
        // as a fn, such as a vector or kw or set
        fn_call = compile_as_fn_call(wasm, resolved_symbol, args);
    }

    release(result.name);
  } else {
    // Not a function or wasm ref, perhaps it's something else we can use as a
    // fn, such as a vector, or kw or set, or perhaps an anonymous lambda eg:
    // (fn [x] x)
    // TODO: make sure is_fn_call is set properly when we use a map as a fn!!!
    fn_call = compile_as_fn_call(wasm, lval_first, args);
  }

  fn_call.is_fn_call = 1;
  return fn_call;
}

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
    init_rest_arg = BinaryenCall(module, "bundle_rest_args", operands, 2,
                                 BinaryenTypeNone());
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
