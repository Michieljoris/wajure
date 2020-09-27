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
#include "state.h"
#include "wasm.h"
#include "wasm_util.h"

void add_partial_fn(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  int partial_fn_index = add_fn_to_table(wasm, "partial_fn");
  if (partial_fn_index != PARTIAL_FN_INDEX)
    quit(wasm,
         "Index of partial_fn in fn table index is not %d!!! This is not "
         "going to work!!!!",
         PARTIAL_FN_INDEX);

  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(0);
  Ber body = BinaryenNop(module);
  BinaryenAddFunction(module, "partial_fn", params_type, results_type, NULL, 0,
                      body);
}

void add_apply_fn(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  int apply_fn_index = add_fn_to_table(wasm, "apply_fn");
  if (apply_fn_index != APPLY_FN_INDEX)
    quit(wasm,
         "Index of apply_fn in fn table index is not %d!!! This is not "
         "going to work!!!!",
         APPLY_FN_INDEX);
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(0);
  Ber body = BinaryenNop(module);
  BinaryenAddFunction(module, "apply_fn", params_type, results_type, NULL, 0,
                      body);
}

void add_call_fn(Wasm* wasm, int n) {
  BinaryenModuleRef module = wasm->module;
  char* fn_name = number_fn_name(wasm, "call");  // call#0 - call#n
  add_fn_to_table(wasm, fn_name);

  // closure pointer, fn_table_index, args_block_ptr and args count
  BinaryenType fn_params_type = make_type_int32(4);
  BinaryenType fn_results_type = make_type_int32(1);
  BinaryenType call_params_type =
      make_type_int32(n + 1);  // first param is closure_pointer
  BinaryenType call_results_type = make_type_int32(1);

  Ber closure_pointer = BinaryenLocalGet(module, 0, BinaryenTypeInt32());
  Ber* operands = malloc((n + 1) * sizeof(Ber));
  operands[0] = closure_pointer;

  Ber args_block_ptr = BinaryenLocalGet(module, 2, BinaryenTypeInt32());
  for (int i = 0; i < n; i++) {
    operands[i + 1] = BinaryenLoad(module, 4, 0, i * 4, 2, BinaryenTypeInt32(),
                                   args_block_ptr);
  }
  Ber wval_fn_table_index = BinaryenLocalGet(module, 1, BinaryenTypeInt32());
  Ber call = BinaryenCallIndirect(module, wval_fn_table_index, operands, n + 1,
                                  call_params_type, call_results_type);
  int children_count = 1;
  Ber children[] = {/* wasm_log_int(wasm, make_int32(module, n)), */
                    /* wasm_log_int(wasm, closure_pointer), */
                    /* wasm_log_int(wasm, wval_fn_table_index), */
                    call};
  Ber body = BinaryenBlock(module, NULL, children, children_count,
                           BinaryenTypeInt32());
  BinaryenAddFunction(module, fn_name, fn_params_type, fn_results_type, NULL, 0,
                      body);
  free(operands);
  release(fn_name);
}

Ber get_wval_prop(BinaryenModuleRef module, Ber wval, char* prop) {
  Ber operands[] = {wval};
  char* fn_call_prefix = "get_wval_";
  char* fn_call = malloc(_strlen(fn_call_prefix) + _strlen(prop) + 1);
  sprintf(fn_call, "%s%s", fn_call_prefix, prop);
  /* printf("wval get call: %s\n", fn_call); */
  Ber ret = BinaryenCall(module, fn_call, operands, 1, make_type_int32(1));
  free(fn_call);
  return ret;
}

Ber validate_as_fn(Wasm* wasm, Ber wval) {
  BinaryenModuleRef module = wasm->module;
  Ber wval_type = get_wval_prop(module, wval, "type");
  Ber is_not_wval_fun = BinaryenBinary(wasm->module, BinaryenNeInt32(),
                                       wval_type, make_int32(module, WVAL_FUN));
  char* error_msg = "not a fn";
  scoped char* str = lalloc_size(_strlen(error_msg));
  sprintf(str, "%s", error_msg);
  Ber not_a_fn_rt_error = wasm_runtime_error(wasm, RT_NOT_A_FN, str).ber;
  Ber all_ok = BinaryenNop(module);
  return BinaryenIf(module, is_not_wval_fun, not_a_fn_rt_error, all_ok);
}

Ber validate_args_count(Wasm* wasm, Ber wval, Ber args_count) {
  BinaryenModuleRef module = wasm->module;
  /* Context* context = wasm->context->car; */
  /* char* fn_name = context->function_context->fn_name; */
  char* error_msg = "fn_name???";
  scoped char* rt_error_msg = lalloc_size(_strlen(error_msg));
  sprintf(rt_error_msg, "%s", error_msg);

  Ber wval_param_count = get_wval_prop(module, wval, "param_count");
  Ber param_count_tee =
      BinaryenLocalTee(module, 2, wval_param_count, BinaryenTypeInt32());
  Ber get_param_count = BinaryenLocalGet(module, 2, BinaryenTypeInt32());
  Ber has_rest_arg = get_wval_prop(module, wval, "has_rest_arg");

  Ber min_param_count = get_wval_prop(module, wval, "min_param_count");
  Ber args_count_lt_min_param_count =
      BinaryenBinary(module, BinaryenLtUInt32(), args_count, min_param_count);
  Ber all_ok = BinaryenNop(module);
  Ber too_few_rt_error =
      wasm_runtime_error(wasm, RT_TOO_FEW_ARGS, rt_error_msg).ber;
  Ber if_args_count_lt_min_param_count = BinaryenIf(
      module, args_count_lt_min_param_count, too_few_rt_error, all_ok);

  Ber args_count_lt_param_count =
      BinaryenBinary(module, BinaryenLtUInt32(), args_count, get_param_count);
  Ber if_args_count_lt_param_count = BinaryenIf(
      module, args_count_lt_param_count,
      wasm_runtime_error(wasm, RT_TOO_FEW_ARGS, rt_error_msg).ber, all_ok);
  Ber args_count_gt_param_count =
      BinaryenBinary(module, BinaryenGtUInt32(), args_count, param_count_tee);
  Ber if_args_count_gt_param_count =
      BinaryenIf(module, args_count_gt_param_count,
                 wasm_runtime_error(wasm, RT_TOO_MANY_ARGS, rt_error_msg).ber,
                 if_args_count_lt_param_count);
  Ber if_has_rest_arg =
      BinaryenIf(module, has_rest_arg, if_args_count_lt_min_param_count,
                 if_args_count_gt_param_count);

  Ber ret = if_has_rest_arg;
  return ret;
}

void add_validate_fn_fn(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;

  Ber children[] = {
      validate_as_fn(wasm, BinaryenLocalGet(module, 0, BinaryenTypeInt32())),
      validate_args_count(wasm,
                          BinaryenLocalGet(module, 0, BinaryenTypeInt32()),
                          BinaryenLocalGet(module, 1, BinaryenTypeInt32()))};
  Ber body = BinaryenBlock(module, NULL, children, 2, BinaryenTypeNone());

  BinaryenType* local_types = make_type_int32_array(1);
  BinaryenAddFunction(module, "validate_fn", make_type_int32(2),
                      BinaryenTypeNone(), local_types, 1, body);
  int validate_fn_index = add_fn_to_table(wasm, "validate_fn");
  if (validate_fn_index != VALIDATE_FN_INDEX)
    quit(wasm,
         "Index of validate_fn in fn table index is not %d!!! This is not "
         "going to work!!!!",
         VALIDATE_FN_INDEX);
}

// This adds MAX_FN_PARAMS wasm fns (call#0-call#20), each fn takes 3 params:
// closure_pointer, fn_table_index and args_block
void add_call_fns(Wasm* wasm) {
  for (int i = 0; i <= MAX_FN_PARAMS; i++) add_call_fn(wasm, i);
}

Ber compile_args_into_block(Wasm* wasm, Ber wval, Cell* args, int args_count,
                            int args_block_ptr_index, LocalIndices* li) {
  BinaryenModuleRef module = wasm->module;
  Ber* children = malloc((2 + args_count) * sizeof(Ber));
  printf("CHILDREN COUNT: %d\n", 2 + args_count);
  int children_count = 0;

  // Put partials on block
  Ber size = BinaryenBinary(module, BinaryenMulInt32(),
                            get_wval_prop(module, wval, "partial_count"),
                            make_int32(module, 4));
  int size_local = li_new(wasm);
  size = BinaryenLocalTee(module, size_local, size, BinaryenTypeInt32());

  // Copy partials to start of args_block
  children[children_count++] = BinaryenMemoryCopy(
      module,
      BinaryenLocalGet(module, args_block_ptr_index, BinaryenTypeInt32()),
      get_wval_prop(module, wval, "partials"), size);

  // Add size of partials to arg block ptr
  int args_block_plus_partials_ptr_index = li_new(wasm);
  children[children_count++] = BinaryenLocalSet(
      module, args_block_plus_partials_ptr_index,
      BinaryenBinary(
          module, BinaryenAddInt32(),
          BinaryenLocalGet(module, args_block_ptr_index, BinaryenTypeInt32()),
          BinaryenLocalGet(module, size_local, BinaryenTypeInt32())));

  int offset = 0;
  // Put args on block ====================================================
  while (args) {
    CResult result = lval_compile(wasm, args->car);
    Ber compiled_arg = result.ber;
    // Any arg that's a result of a fn call will be released
    if (result.is_fn_call) {
      int local_index = li_track(wasm, li, li_new(wasm));
      compiled_arg = BinaryenLocalTee(module, local_index, compiled_arg,
                                      BinaryenTypeInt32());
    }
    // Put compiled arg on args block
    Ber args_block_ptr = BinaryenLocalGet(
        module, args_block_plus_partials_ptr_index, BinaryenTypeInt32());
    children[children_count++] =
        BinaryenStore(module, 4, offset, 2, args_block_ptr, compiled_arg,
                      BinaryenTypeInt32());
    // And loop
    offset += 4;
    args = args->cdr;
  }
  Ber block = BinaryenBlock(module, uniquify_name(wasm, "args"), children,
                            children_count, BinaryenTypeNone());
  free(children);
  return block;
}

Ber call_bundle_rest_args(Wasm* wasm, int args_block_ptr_index, Ber args_count,
                          Ber wval_rest_arg_index) {
  BinaryenModuleRef module = wasm->module;

  int local_index = li_new(wasm);
  wval_rest_arg_index = BinaryenLocalTee(
      module, local_index, wval_rest_arg_index, BinaryenTypeInt32());
  Ber offset = BinaryenBinary(module, BinaryenMulInt32(), make_int32(module, 4),
                              wval_rest_arg_index);

  Ber args_block_ptr =
      BinaryenLocalGet(module, args_block_ptr_index, BinaryenTypeInt32());
  Ber rest_args_start =
      BinaryenBinary(module, BinaryenAddInt32(), args_block_ptr, offset);

  Ber get_wval_rest_arg_index =
      BinaryenLocalGet(module, local_index, BinaryenTypeInt32());
  Ber rest_arg_length = BinaryenBinary(module, BinaryenSubInt32(), args_count,
                                       get_wval_rest_arg_index);
  Ber operands[] = {rest_args_start, rest_arg_length};
  return BinaryenCall(module, "bundle_rest_args", operands, 2,
                      BinaryenTypeNone());
}

Ber call_fn_by_ref(Wasm* wasm, Ber wval, Cell* args, int args_block_ptr_index,
                   Ber* block_children, int* block_children_count,
                   LocalIndices* li) {
  BinaryenModuleRef module = wasm->module;
  Ber wval_fn_index = get_wval_prop(module, wval, "fn_table_index");
  Ber wval_closure = get_wval_prop(module, wval, "closure");
  Ber wval_param_count = get_wval_prop(module, wval, "param_count");
  Ber wval_has_rest_arg = get_wval_prop(module, wval, "has_rest_arg");
  Ber wval_rest_arg_index = get_wval_prop(module, wval, "rest_arg_index");
  int args_count = list_count(args);

  // Add partial count and args count together for total args count
  int total_args_count_local = li_new(wasm);
  Ber total_args_count =
      BinaryenBinary(module, BinaryenAddInt32(), make_int32(module, args_count),
                     get_wval_prop(module, wval, "partial_count"));
  block_children[(*block_children_count)++] =
      BinaryenLocalSet(module, total_args_count_local, total_args_count);

  // Validate the fn as fn and number of args
  if (wasm->validate_fn_at_rt) {
    Ber operands[] = {wval, BinaryenLocalGet(module, total_args_count_local,
                                             BinaryenTypeInt32())};
    Ber call_validate_fn = BinaryenCallIndirect(
        module, make_int32(module, VALIDATE_FN_INDEX), operands, 2,
        make_type_int32(2), BinaryenTypeNone());
    block_children[(*block_children_count)++] = call_validate_fn;
  }

  // Load partials and args into the args block
  Ber args_block = compile_args_into_block(wasm, wval, args, args_count,
                                           args_block_ptr_index, li);
  block_children[(*block_children_count)++] = args_block;

  // If there is a rest arg, bundle it into the last arg
  Ber bundle_rest_args = call_bundle_rest_args(
      wasm, args_block_ptr_index,
      BinaryenLocalGet(module, total_args_count_local, BinaryenTypeInt32()),
      wval_rest_arg_index);
  bundle_rest_args = BinaryenIf(module, wval_has_rest_arg, bundle_rest_args,
                                BinaryenNop(module));
  block_children[(*block_children_count)++] = bundle_rest_args;

  // Pass closure pointer, fn_table_index, ptr to args block and count of args
  // to redirecting wasm call fn
  Ber call_operands[] = {
      wval_closure, wval_fn_index,
      BinaryenLocalGet(module, args_block_ptr_index, BinaryenTypeInt32()),
      BinaryenLocalGet(module, total_args_count_local, BinaryenTypeInt32())};

  // The first MAX_FN_PARAMS (20) fns in the global fn table redirect to the
  // real fn with index wval_fn_index, loading the right number of args from the
  // args block.
  Ber fn_table_index = wval_param_count;
  return BinaryenCallIndirect(module, fn_table_index, call_operands, 4,
                              make_type_int32(4), BinaryenTypeInt32());
}

Ber* compile_args_into_operands(Wasm* wasm, char* fn_name, Lval* lval_fun,
                                Cell* args, LocalIndices* li) {
  BinaryenModuleRef module = wasm->module;
  int param_count = lval_fun->param_count;
  int has_rest_arg = lval_fun->rest_arg_index;
  int min_param_count = has_rest_arg ? param_count - 1 : param_count;
  int args_count = list_count(args);
  int partial_count = list_count(lval_fun->partials);
  int total_args_count = args_count + partial_count;

  // Check total number of args (partials plus args) matches signature of
  // function
  if (has_rest_arg) {
    if (total_args_count < min_param_count)
      quit(wasm, "Not enough args passed to %s (need at least %d)", fn_name,
           min_param_count);
  } else {
    if (total_args_count != param_count)
      quit(wasm, "Wrong number of args (%d) passed to %s (%d expected)",
           total_args_count, fn_name, param_count);
  }

  Ber* compiled_args = malloc(sizeof(Ber) * total_args_count);
  int compiled_args_count = 0;

  // Grab and datafy the partials from the function
  Cell* head = lval_fun->partials;
  while (head) {
    Ber compiled_arg = datafy_lval(wasm, head->car, NULL).ber;
    // We need to retain datafied values if the value ends up in the rest arg
    // because it will be released when we're done with this fn call
    if (compiled_args_count >= min_param_count)
      compiled_arg = wasm_retain(wasm, compiled_arg).ber;
    /* printf("partial compiled_arg %d :", compiled_args_count); */
    /* lval_println(head->car); */
    compiled_args[compiled_args_count++] = compiled_arg;
    head = head->cdr;
  }

  // And grab and compile the args
  head = args;
  while (head) {
    CResult result = lval_compile(wasm, head->car);
    Ber compiled_arg = result.ber;
    if (compiled_args_count < min_param_count) {
      // If the arg is not part of the rest arg we'll release it when we're done
      // with the fn call
      if (result.is_fn_call) {
        int local_index = li_track(wasm, li, li_new(wasm));
        compiled_arg = BinaryenLocalTee(module, local_index, compiled_arg,
                                        BinaryenTypeInt32());
      }
    } else {
      // If the arg *is* part of the rest arg we retain any data values, since
      // the rest arg list will be released
      if (!result.is_fn_call)
        compiled_arg = wasm_retain(wasm, compiled_arg).ber;
    }
    /* printf("regular compiled_arg %d :", compiled_args_count); */
    /* lval_println(head->car); */
    compiled_args[compiled_args_count++] = compiled_arg;
    head = head->cdr;
  }

  // Let's make the operands for the fn call
  Ber* call_operands = malloc((1 + param_count) * sizeof(Ber));
  // First arg is always a closure pointer
  Ber closure_pointer = make_int32(module, 0);
  int call_operands_count = 0;
  call_operands[call_operands_count++] = closure_pointer;

  // Add all the args that are definitely not part of any rest arg.
  for (int i = 0; i < min_param_count; i++)
    call_operands[call_operands_count++] = compiled_args[i];

  // Collect the remainder in the rest arg
  if (has_rest_arg) {
    int rest_arg_count = total_args_count - min_param_count;
    if (rest_arg_count) {
      /* printf("compiled-args count %d\n", compiled_args_count); */
      /* printf("rest arg count %d\n", rest_arg_count); */
      // Make a chain of cells, one cell per extra arg
      Ber head = make_int32(module, 0);
      int i = total_args_count - 1;
      do {
        Ber operands[] = {compiled_args[i], head};
        printf("Making new cell %d\n ", i);
        head =
            BinaryenCall(module, "new_cell", operands, 2, BinaryenTypeInt32());
        i--;
      } while (i >= min_param_count);
      // Assign the head to a new list
      Ber lval_list_operands[1] = {head};
      // Make sure it gets released when we're done
      int wasm_lval_list_index = li_track(wasm, li, li_new(wasm));
      Ber wasm_lval_list = BinaryenCall(
          module, "new_lval_list", lval_list_operands, 1, make_type_int32(1));
      wasm_lval_list = BinaryenLocalTee(module, wasm_lval_list_index,
                                        wasm_lval_list, BinaryenTypeInt32());
      // And add the rest arg list to our operands
      call_operands[call_operands_count++] = wasm_lval_list;
    } else
      // Put a nil in the rest arg if there's no args left
      call_operands[call_operands_count++] = datafy_nil(wasm).ber;
  }
  free(compiled_args);
  return call_operands;
}

Ber call_fn_by_name(Wasm* wasm, char* fn_name, Lval* lval_fun, Cell* args,
                    LocalIndices* li) {
  Ber* call_operands =
      compile_args_into_operands(wasm, fn_name, lval_fun, args, li);

  int param_count = 1 + lval_fun->param_count;  // closure_ptr + args
  Ber result = BinaryenCall(wasm->module, fn_name, call_operands, param_count,
                            BinaryenTypeInt32());
  free(call_operands);
  return result;
}

Ber call_global_fn(Wasm* wasm, Ber fn_table_index, Lval* lval_fun, Cell* args,
                   LocalIndices* li) {
  Ber* call_operands =
      compile_args_into_operands(wasm, lval_fun->str, lval_fun, args, li);
  int param_count = 1 + lval_fun->param_count;  // closure_ptr + args
  Ber result = BinaryenCallIndirect(wasm->module, fn_table_index, call_operands,
                                    param_count, make_type_int32(param_count),
                                    BinaryenTypeInt32());
  free(call_operands);
  return result;
}

// TODO: wval is a precompiled ber!!! Binaryen says don't do that for
// optimisation reasons
CResult apply(Wasm* wasm, int fn_ref_type, union FnRef fn_ref, Lval* lval_fun,
              Cell* args) {
  BinaryenModuleRef module = wasm->module;
  Context* context = wasm->context->car;

  int block_children_count = 0;

  // put partials on stack, args, release_locals, result
  int block_size = 10;  // TODO: ?????????????????
  /* int block_size = 99;       // TODO: for debugging */
  int max_block_children_count = list_count(args) + block_size;
  Ber* block_children = malloc(sizeof(Ber) * max_block_children_count);

  LocalIndices* li = li_init();

  int args_block_ptr_index;
  // Call fn ================================================================
  Ber result = NULL;
  switch (fn_ref_type) {
    case BER: {
      Ber wval = fn_ref.ber;
      // TODO: implement using maps and sets and vectors and keywords as fns.
      /* Ber get_type_call = get_wval_prop(module, wval, "type"); */
      /* block_children[block_children_count++] = */
      /*     wasm_log_int(wasm, get_type_call); */
      int args_count = list_count(args);
      Ber operands[] = {make_int32(module, args_count)};
      Ber args_block_ptr =
          BinaryenCall(module, "lalloc_size", operands, 1, BinaryenTypeInt32());
      args_block_ptr_index = li_new(wasm);
      Ber local_set_args_block_ptr =
          BinaryenLocalSet(module, args_block_ptr_index, args_block_ptr);
      block_children[block_children_count++] = local_set_args_block_ptr;

      result = call_fn_by_ref(wasm, wval, args, args_block_ptr_index,
                              block_children, &block_children_count, li);

    } break;
    case FN_NAME: {
      // TODO: add partials to args
      result = call_fn_by_name(wasm, fn_ref.fn_name, lval_fun, args, li);
    } break;
    case GLOBAL: {
      // TODO: add partials to args
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

  if (fn_ref_type == BER) {
    Ber args_block_ptr =
        BinaryenLocalGet(module, args_block_ptr_index, BinaryenTypeInt32());
    block_children[block_children_count++] =
        wasm_release(wasm, args_block_ptr).ber;
  }

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
            printf("APPLY LAMBDA %s :", lval_first->str);
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
            fn_call.is_fn_call = 1;
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
        // reduce it to a compile_list/lambda_call. However the default case
        // is that we cannot assume anything about the lval.

        Ber compiled_symbol =
            compile_lval_ref(wasm, lval_first, resolved_symbol).ber;

        union FnRef fn_ref = {.ber = compiled_symbol};
        fn_call = apply(wasm, BER, fn_ref, NULL, args);
        fn_call.is_fn_call = 1;
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
        fn_call.is_fn_call = 1;
    }

    release(result.name);
  } else {
    // Not a function or wasm ref, perhaps it's something else we can use as a
    // fn, such as a vector, or kw or set, or perhaps an anonymous lambda eg:
    // (fn [x] x)
    // TODO: make sure is_fn_call is set properly when we use a map as a fn!!!
    fn_call = compile_as_fn_call(wasm, lval_first, args);
    fn_call.is_fn_call = 1;
  }

  return fn_call;
}

/* CResult wasm_process_args(Wasm* wasm, int param_count, int rest_arg_index)
 * {
 */
/*   BinaryenModuleRef module = wasm->module; */
/*   Context* context = wasm->context->car; */
/*   char* fn_name = context->function_context->fn_name; */
/*   Ber args_count = BinaryenLocalGet( */
/*       wasm->module, 1, */
/*       BinaryenTypeInt32());  // second param to fn is args count */

/*   Ber check_args_count = NULL; */
/*   if (wasm->runtime_check_args_count && rest_arg_index != 1) { */
/*     Ber wasm_param_count = make_int32(module, param_count); */
/*     Ber check_operands[3] = {wasm_param_count, args_count, */
/*                              make_int32(module, rest_arg_index)}; */
/*     Ber check_args_count_call_result = BinaryenCall( */
/*         module, "check_args_count", check_operands, 3,
 * BinaryenTypeInt32());
 */
/*     BinaryenOp eq = BinaryenEqInt32(); */
/*     Ber is_too_few_args = */
/*         BinaryenBinary(module, eq, check_args_count_call_result, */
/*                        make_int32(module, TOO_FEW_ARGS)); */
/*     Ber is_too_many_args = */
/*         BinaryenBinary(module, eq, check_args_count_call_result, */
/*                        make_int32(module, TOO_MANY_ARGS)); */
/*     Ber all_ok = BinaryenNop(module); */
/*     Ber if_too_few = BinaryenIf( */
/*         module, is_too_few_args, */
/*         wasm_runtime_error(wasm, RT_TOO_FEW_ARGS, fn_name).ber, all_ok); */
/*     Ber if_too_many = BinaryenIf( */
/*         module, is_too_many_args, */
/*         wasm_runtime_error(wasm, RT_TOO_MANY_ARGS, fn_name).ber,
 * if_too_few);
 */

/*     check_args_count = if_too_many; */
/*   } */

/*   Ber init_rest_arg = NULL; */
/*   if (rest_arg_index) { */
/*     Ber offset = BinaryenBinary(module, BinaryenMulInt32(), */
/*                                 make_int32(module, 4), args_count); */

/*     Ber stack_pointer = */
/*         BinaryenGlobalGet(wasm->module, "stack_pointer",
 * BinaryenTypeInt32()); */
/*     Ber args_start = */
/*         BinaryenBinary(module, BinaryenSubInt32(), stack_pointer, offset);
 */

/*     Ber rest_arg_length = */
/*         BinaryenBinary(module, BinaryenSubInt32(), args_count, */
/*                        make_int32(module, rest_arg_index - 1)); */
/*     Ber operands[] = {args_start, rest_arg_length}; */
/*     init_rest_arg = BinaryenCall(module, "bundle_rest_args", operands, 2,
 */
/*                                  BinaryenTypeNone()); */
/*     if (check_args_count == NULL) return cresult(init_rest_arg); */
/*   } */
/*   if (check_args_count) { */
/*     if (!init_rest_arg) return cresult(check_args_count); */
/*     Ber block_children[2] = {check_args_count, init_rest_arg}; */
/*     return cresult(BinaryenBlock(module, "process_args", block_children, 2,
 */
/*                                  BinaryenTypeNone())); */
/*   } */
/*   return cnull(); */
/* } */
