#include "native.h"

#include "compile.h"
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

void add_call_fn(Wasm* wasm, int n) {
  BinaryenModuleRef module = wasm->module;
  char* fn_name = number_fn_name(wasm, "call");  // call#0 - call#n
  add_fn_to_table(wasm, fn_name);

  // wval, args_block_ptr and args count
  int wval_param = 0;
  int args_block_ptr_param = 1;
  int args_count_param = 2;

  int body_children_count = 0;
  Ber* body_children = malloc(10 * sizeof(Ber));  // TODO: 10 ????

  // Validate the fn as fn and number of args
  if (wasm->validate_fn_at_rt) {
    /* Ber validate_operands[] = {local_get_int32(module, wval_param), */
    /*                            local_get_int32(module, args_count_param)}; */

    /* NativeFn* native_fn = */
    /*     alist_get(wasm->wajure_to_native_fn_map, is_eq_str, "validate_fn");
     */
    /* Ber call_validate_fn = BinaryenCallIndirect( */
    /*     module, make_int32(module, native_fn->fn_table_index), */
    /*     validate_operands, 2, make_type_int32(2), BinaryenTypeNone()); */
    /* body_children[body_children_count++] = call_validate_fn; */
    body_children[body_children_count++] =
        validate_as_fn(wasm, local_get_int32(module, wval_param));
  }

  // If there is a rest arg, bundle it into the last arg
  Ber bundle_operands[] = {local_get_int32(module, wval_param),
                           local_get_int32(module, args_block_ptr_param),
                           local_get_int32(module, args_count_param)};
  body_children[body_children_count++] = BinaryenCall(
      module, "bundle_rest_args", bundle_operands, 3, BinaryenTypeNone());

  // Load args into operands of call to fn
  Ber closure_pointer =
      get_wval_prop(module, local_get_int32(module, wval_param), "closure");
  Ber* operands = malloc((n + 1) * sizeof(Ber));
  operands[0] = closure_pointer;
  Ber args_block_ptr = local_get_int32(module, args_block_ptr_param);
  for (int i = 0; i < n; i++) {
    operands[i + 1] = BinaryenLoad(module, 4, 0, i * 4, 2, BinaryenTypeInt32(),
                                   args_block_ptr);
  }

  // Call fn
  Ber wval_fn_table_index = get_wval_prop(
      module, local_get_int32(module, wval_param), "fn_table_index");
  BinaryenType call_params_type =
      make_type_int32(n + 1);  // first param is closure_pointer
  BinaryenType call_results_type = make_type_int32(1);
  Ber call = BinaryenCallIndirect(module, wval_fn_table_index, operands, n + 1,
                                  call_params_type, call_results_type);
  body_children[body_children_count++] = call;
  Ber body = BinaryenBlock(module, NULL, body_children, body_children_count,
                           BinaryenTypeInt32());

  // Add function to wasm
  BinaryenType fn_params_type = make_type_int32(3);
  BinaryenType fn_results_type = make_type_int32(1);
  BinaryenAddFunction(module, fn_name, fn_params_type, fn_results_type, NULL, 0,
                      body);
  free(operands);
  free(body_children);
  release(fn_name);
}

// This adds MAX_FN_PARAMS wasm fns (call#0-call#20), each fn takes 3 params:
// closure_pointer, fn_table_index and args_block
void add_call_fns(Wasm* wasm) {
  for (int i = 0; i <= MAX_FN_PARAMS; i++) add_call_fn(wasm, i);
}

void add_bundle_rest_arg_fn(Wasm* wasm, int n) {
  BinaryenModuleRef module = wasm->module;
  char* fn_name = number_fn_name(
      wasm, "bundle_rest_arg");  // bundle_rest-arg#21 - bundle_rest_arg#41
  add_fn_to_table(wasm, fn_name);

  // wval, args_block_ptr and args count
  int wval_param = 0;
  int args_block_ptr_param = 1;
  int args_count_param = 2;

  int body_children_count = 0;
  Ber* body_children = malloc(10 * sizeof(Ber));  // TODO: 10 ?????

  // If there is a rest arg, bundle it into the last arg
  Ber bundle_operands[] = {local_get_int32(module, wval_param),
                           local_get_int32(module, args_block_ptr_param),
                           local_get_int32(module, args_count_param)};
  body_children[body_children_count++] = BinaryenCall(
      module, "bundle_rest_args", bundle_operands, 3, BinaryenTypeNone());

  // Forward to call fn (which loads the args)
  Ber wval_fn_table_index = make_int32(module, n);
  BinaryenType call_params_type = make_type_int32(3);
  BinaryenType call_results_type = make_type_int32(1);
  Ber operands[] = {local_get_int32(module, wval_param),
                    local_get_int32(module, args_block_ptr_param),
                    local_get_int32(module, args_count_param)};
  Ber call = BinaryenCallIndirect(module, wval_fn_table_index, operands, 3,
                                  call_params_type, call_results_type);
  body_children[body_children_count++] = call;
  Ber body = BinaryenBlock(module, NULL, body_children, body_children_count,
                           BinaryenTypeInt32());

  // Add function to wasm
  BinaryenType fn_params_type = make_type_int32(3);
  BinaryenType fn_results_type = make_type_int32(1);
  BinaryenAddFunction(module, fn_name, fn_params_type, fn_results_type, NULL, 0,
                      body);
  free(body_children);
  release(fn_name);
}

void add_bundle_rest_arg_fns(Wasm* wasm) {
  for (int i = 0; i <= MAX_FN_PARAMS; i++) add_bundle_rest_arg_fn(wasm, i);
}

void add_rt_error_too_few_args_fn(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;

  int body_children_count = 0;
  Ber* body_children = malloc(10 * sizeof(Ber));  // TODO: 10 ?????
  char* error_msg = "fn_name???";
  scoped char* rt_error_msg = lalloc_size(_strlen(error_msg));
  sprintf(rt_error_msg, "%s", error_msg);

  Ber too_few_rt_error =
      wasm_runtime_error(wasm, RT_TOO_FEW_ARGS, rt_error_msg).ber;

  body_children[body_children_count++] = too_few_rt_error;
  body_children[body_children_count++] = make_int32(module, 0);
  Ber body = BinaryenBlock(module, NULL, body_children, body_children_count,
                           BinaryenTypeInt32());
  // Add function to wasm
  char* fn_name = "rt_error_too_few_args";
  BinaryenType fn_params_type = make_type_int32(3);
  BinaryenType fn_results_type = make_type_int32(1);
  BinaryenAddFunction(module, fn_name, fn_params_type, fn_results_type, NULL, 0,
                      body);
}

void add_rt_error_too_many_args_fn(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;

  int body_children_count = 0;
  Ber* body_children = malloc(10 * sizeof(Ber));  // TODO: 10 ?????

  char* error_msg = "fn_name???";
  scoped char* rt_error_msg = lalloc_size(_strlen(error_msg));
  sprintf(rt_error_msg, "%s", error_msg);

  Ber too_many_rt_error =
      wasm_runtime_error(wasm, RT_TOO_MANY_ARGS, rt_error_msg).ber;
  body_children[body_children_count++] = too_many_rt_error;
  body_children[body_children_count++] = make_int32(module, 0);
  Ber body = BinaryenBlock(module, NULL, body_children, body_children_count,
                           BinaryenTypeInt32());

  // Add function to wasm
  char* fn_name = "rt_error_too_many_args";
  BinaryenType fn_params_type = make_type_int32(3);
  BinaryenType fn_results_type = make_type_int32(1);
  BinaryenAddFunction(module, fn_name, fn_params_type, fn_results_type, NULL, 0,
                      body);
}

// Wasm fn that receives ptr to wval_fn in local 0, and args count in local 1
// and throws runtime error when args count is invalid for  the wval_fn
Ber validate_args_count(Wasm* wasm, Ber wval, Ber args_count) {
  BinaryenModuleRef module = wasm->module;
  /* Context* context = wasm->context->car; */
  /* char* fn_name = context->function_context->fn_name; */
  char* error_msg = "fn_name???";
  scoped char* rt_error_msg = lalloc_size(_strlen(error_msg));
  sprintf(rt_error_msg, "%s", error_msg);

  // Put the param count of the wval_fn in local 2
  Ber wval_param_count = get_wval_prop(module, wval, "param_count");
  Ber param_count_tee =
      BinaryenLocalTee(module, 2, wval_param_count, BinaryenTypeInt32());
  Ber get_param_count = BinaryenLocalGet(module, 2, BinaryenTypeInt32());

  // has_rest_arg
  Ber has_rest_arg = get_wval_prop(module, wval, "has_rest_arg");
  // min param_count
  Ber min_param_count = get_wval_prop(module, wval, "min_param_count");

  // Check if args count is less than min param count (so param count minus any
  // rest param)
  Ber args_count_lt_min_param_count =
      BinaryenBinary(module, BinaryenLtUInt32(), args_count, min_param_count);
  Ber all_ok = BinaryenNop(module);
  Ber too_few_rt_error =
      wasm_runtime_error(wasm, RT_TOO_FEW_ARGS, rt_error_msg).ber;
  Ber if_args_count_lt_min_param_count = BinaryenIf(
      module, args_count_lt_min_param_count, too_few_rt_error, all_ok);

  // Check if args count is less than param count (in case there's no rest
  // param)
  Ber args_count_lt_param_count =
      BinaryenBinary(module, BinaryenLtUInt32(), args_count, get_param_count);
  Ber if_args_count_lt_param_count = BinaryenIf(
      module, args_count_lt_param_count,
      wasm_runtime_error(wasm, RT_TOO_FEW_ARGS, rt_error_msg).ber, all_ok);

  // if args count > param count throw rt_error else check if less than param
  // count
  Ber args_count_gt_param_count =
      BinaryenBinary(module, BinaryenGtUInt32(), args_count, param_count_tee);
  Ber if_args_count_gt_param_count =
      BinaryenIf(module, args_count_gt_param_count,
                 wasm_runtime_error(wasm, RT_TOO_MANY_ARGS, rt_error_msg).ber,
                 if_args_count_lt_param_count);
  // Result is as this pseudo code:
  /* if (has_rest_arg) { */
  /*   if (args_count < min_param_count) rt_error */
  /*   } else { */
  /*   if (args_count > param_count) throw rt_error */
  /*   else if (args_count < param_count) throw rt_error */
  /* } */
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
  /* int validate_fn_index = add_fn_to_table(wasm, "validate_fn"); */
  /* if (validate_fn_index != VALIDATE_FN_INDEX) */
  /*   quit(wasm, */
  /*        "Index of validate_fn in fn table index is not %d!!! This is not "
   */
  /*        "going to work!!!!", */
  /*        VALIDATE_FN_INDEX); */
}

void add_copy_and_retain_fn(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  int src_local = 0;
  int src_count_local = 1;
  int dest_local = 2;

  // Loop over partials in source
  char* loop = uniquify_name(wasm, "copy_and_retain");
  Ber* c = malloc(10 * sizeof(Ber));  // TODO 10 ??
  int c_count = 0;

  // Get partial from src and copy to dest
  Ber get_ptr = BinaryenLoad(module, 4, 0, 0, 2, BinaryenTypeInt32(),
                             local_get_int32(module, src_local));
  get_ptr = wasm_retain(wasm, get_ptr).ber;
  Ber copy_ptr =
      BinaryenStore(module, 4, 0, 2, local_get_int32(module, dest_local),
                    get_ptr, BinaryenTypeInt32());
  c[c_count++] = copy_ptr;

  // Decrease our counter so we know when stop continuing the loop
  Ber dec_src_count =
      BinaryenLocalSet(module, src_count_local,
                       BinaryenBinary(module, BinaryenSubInt32(),
                                      local_get_int32(module, src_count_local),
                                      make_int32(module, 1)));
  c[c_count++] = dec_src_count;

  // Increase src and dest pointer by 4
  Ber add_4_to_src_ptr =
      BinaryenBinary(module, BinaryenAddInt32(),
                     local_get_int32(module, src_local), make_int32(module, 4));
  add_4_to_src_ptr = BinaryenLocalSet(module, src_local, add_4_to_src_ptr);
  c[c_count++] = add_4_to_src_ptr;
  Ber add_4_to_dest_ptr = BinaryenBinary(module, BinaryenAddInt32(),
                                         local_get_int32(module, dest_local),
                                         make_int32(module, 4));
  add_4_to_dest_ptr = BinaryenLocalSet(module, dest_local, add_4_to_dest_ptr);
  c[c_count++] = add_4_to_dest_ptr;

  // And add a break (jump) to the loop beginning at the end of our loop block
  Ber continue_loop = BinaryenBreak(module, loop, NULL, NULL);
  c[c_count++] = continue_loop;

  Ber loop_block = BinaryenBlock(module, NULL, c, c_count, BinaryenTypeNone());

  // But only run loop block if src_count is not zero
  Ber body = BinaryenIf(module, local_get_int32(module, src_count_local),
                        loop_block, local_get_int32(module, dest_local));

  // Add wasm fn
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(1);

  int locals_count = 0;
  BinaryenType* local_types = NULL;

  char* fn_name = "copy_and_retain";
  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, BinaryenLoop(module, loop, body));

  /* int copy_and_retain_fn_index = add_fn_to_table(wasm, fn_name); */
  /* if (copy_and_retain_fn_index != COPY_AND_RETAIN_FN_INDEX) */
  /*   quit(wasm, */
  /*        "Index of copy_and_retain_fn in fn table index is not %d!!! This is
   * " */
  /*        "not " */
  /*        "going to work!!!!", */
  /*        copy_and_retain_fn_index); */
}

// Put the compiled args in a block (lalloc_size) and pass them to our
// native partial fn
// Check for type:
// 1. LVAL_WASM_LAMBDA -> add partials to a copy of the lval_wasm_lambda
// of fn and args to partial, return it
// 2.TODO: vector/set/map -> make lval_wasm_lambda of get and args to
// partial fn as partials
// 3. anything else: return as is

void add_partial_fn(Wasm* wasm) {
  CONTEXT_FUNCTION("add_partial_fn", "add_partail_fn", 4)

  int wasm_params_count = 3;

  /* int wval_param = 0; */
  int args_block_ptr_param = 1;
  int args_count_param = 2;

  BinaryenModuleRef module = wasm->module;

  Ber* children = malloc(100 * sizeof(Ber));  // TODO: 10 ???
  int children_count = 0;

  // Get pointer to wval_fn
  int wval_local = li_new(wasm);
  Ber wval_load = BinaryenLoad(module, 4, 0, 0, 2, BinaryenTypeInt32(),
                               local_get_int32(module, args_block_ptr_param));

  children[children_count++] = BinaryenLocalSet(module, wval_local, wval_load);
  // TODO: make sure it's a wval_fn!!!! Or something that can be turned into a
  // fn, such as {},[],:kw etc

  // TODO: if fn is not an function we want to return just the value itself,
  // retained, since at compile time we don't know if it's been a real fn
  // call, so we'll pretend it was, and the result of the whole partial call
  // can then be released when required.

  // Calculate new partial count (partial count of fn plus count of args - 1)
  Ber ber_args_count = local_get_int32(module, args_count_param);
  Ber partial_count = BinaryenBinary(module, BinaryenSubInt32(), ber_args_count,
                                     make_int32(module, 1));
  int partial_count_local = li_new(wasm);
  Ber partial_count_tee = BinaryenLocalTee(module, partial_count_local,
                                           partial_count, BinaryenTypeInt32());

  int wval_partial_count_local = li_new(wasm);
  Ber wval_partial_count_tee = BinaryenLocalTee(
      module, wval_partial_count_local,
      get_wval_prop(module, local_get_int32(module, wval_local),
                    "partial_count"),
      BinaryenTypeInt32());
  Ber total_partial_count = BinaryenBinary(
      module, BinaryenAddInt32(), wval_partial_count_tee, partial_count_tee);

  int total_partial_count_local = li_new(wasm);
  Ber total_partial_count_tee =
      BinaryenLocalTee(module, total_partial_count_local, total_partial_count,
                       BinaryenTypeInt32());

  // Reserve a block of memory for the partials
  Ber lalloc_size_operands[] = {BinaryenBinary(module, BinaryenMulInt32(),
                                               total_partial_count_tee,
                                               make_int32(module, 4))};
  Ber lalloc_partials = BinaryenCall(
      module, "lalloc_size", lalloc_size_operands, 1, BinaryenTypeInt32());
  int partials_local = li_new(wasm);
  Ber set_partials_local =
      BinaryenLocalSet(module, partials_local, lalloc_partials);
  children[children_count++] = set_partials_local;

  // Copy partials of fn into this block, retaining them -------------------
  Ber copy_and_retain_operands[] = {
      get_wval_prop(module, local_get_int32(module, wval_local),
                    "partials"),                          // partials of old fn
      local_get_int32(module, wval_partial_count_local),  // old partial count
      local_get_int32(module, partials_local)};  // ptr to partials of new fn

  NativeFn* native_fn =
      alist_get(wasm->wajure_to_native_fn_map, is_eq_str, "copy_and_retain");
  Ber copy_and_retain_fn_index = make_int32(module, native_fn->fn_table_index);
  Ber copy_wval_partials = BinaryenCallIndirect(
      module, copy_and_retain_fn_index, copy_and_retain_operands, 3,
      make_type_int32(3), BinaryenTypeInt32());

  // Copy rest of args of fn to our partial fn's partials, retaining them as
  // well
  Ber foo = BinaryenBinary(module, BinaryenAddInt32(),
                           local_get_int32(module, args_block_ptr_param),
                           make_int32(module, 4));
  Ber copy_and_retain_operands2[] = {
      foo, local_get_int32(module, partial_count_local),
      copy_wval_partials};  // dest

  Ber copy_args = BinaryenCallIndirect(module, copy_and_retain_fn_index,
                                       copy_and_retain_operands2, 3,
                                       make_type_int32(3), BinaryenTypeInt32());
  copy_args = BinaryenDrop(module, copy_args);
  children[children_count++] = copy_args;

  // And put rest of operands for make_lval_wasm_lambda together
  Ber wval_fn_table_index = get_wval_prop(
      module, local_get_int32(module, wval_local), "fn_table_index");
  Ber wval_closure =
      wasm_retain(
          wasm,
          get_wval_prop(module, local_get_int32(module, wval_local), "closure"))
          .ber;
  Ber wval_param_count =
      get_wval_prop(module, local_get_int32(module, wval_local), "param_count");
  Ber wval_has_rest_arg = get_wval_prop(
      module, local_get_int32(module, wval_local), "has_rest_arg");

  Ber operands[6] = {
      wval_fn_table_index,
      wval_param_count,
      wval_has_rest_arg,
      wval_closure,
      local_get_int32(module, partials_local),
      local_get_int32(module, total_partial_count_local),
  };

  // Make the new wval_fn
  Ber make_lval_wasm_lambda_call = BinaryenCall(
      wasm->module, "make_lval_wasm_lambda", operands, 6, make_type_int32(1));
  children[children_count++] = make_lval_wasm_lambda_call;
  Ber block = BinaryenBlock(module, NULL, children, children_count,
                            BinaryenTypeInt32());

  // And add the partial fn to wasm
  char* fn_name = "partial";
  /* int partial_fn_index = add_fn_to_table(wasm, fn_name); */
  /* if (partial_fn_index != PARTIAL_FN_INDEX) */
  /*   quit(wasm, */
  /*        "Index of partial_fn in fn table index is not %d!!! This is not " */
  /*        "going to work!!!!", */
  /*        PARTIAL_FN_INDEX); */

  BinaryenType params_type = make_type_int32(wasm_params_count);
  BinaryenType results_type = make_type_int32(1);

  int locals_count = context->function_context->local_count - wasm_params_count;
  BinaryenType* local_types = make_type_int32_array(locals_count);

  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, block);
}

void add_apply_fn(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  char* fn_name = "apply";
  /* int apply_fn_index = add_fn_to_table(wasm, fn_name); */
  /* if (apply_fn_index != APPLY_FN_INDEX) */
  /*   quit(wasm, */
  /*        "Index of apply_fn in fn table index is not %d!!! This is not " */
  /*        "going to work!!!!", */
  /*        APPLY_FN_INDEX); */
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(0);
  Ber body = BinaryenNop(module);
  BinaryenAddFunction(module, fn_name, params_type, results_type, NULL, 0,
                      body);
}

CResult test_native_partial_fn(Wasm* wasm, CResult fn_arg, Cell* args,
                               int args_count) {
  BinaryenModuleRef module = wasm->module;
  /* LocalIndices* li = li_init(); */
  int offset = 0;

  // Allocate an arg block
  Ber operands[] = {make_int32(module, args_count * 4)};
  Ber args_block_ptr =
      BinaryenCall(module, "lalloc_size", operands, 1, BinaryenTypeInt32());
  int args_block_ptr_local = li_new(wasm);
  Ber local_tee_args_block_ptr = BinaryenLocalTee(
      module, args_block_ptr_local, args_block_ptr, BinaryenTypeInt32());

  Ber* children = malloc((10 + args_count) * sizeof(Ber));  // TODO: 10 ??
  int children_count = 0;

  // First arg is the fn
  children[children_count++] =
      BinaryenStore(module, 4, offset, 2, local_tee_args_block_ptr, fn_arg.ber,
                    BinaryenTypeInt32());
  offset += 4;

  // Compile rest of args and add them to the args block
  while (args) {
    CResult result = lval_compile(wasm, args->car);
    Ber compiled_arg = result.ber;

    // Any arg that's a result of a fn call will be released
    if (result.is_fn_call) {
      /* int local_index = li_track(wasm, li, li_new(wasm)); */
      /* compiled_arg = BinaryenLocalTee(module, local_index, compiled_arg,
       */
      /*                                 BinaryenTypeInt32()); */
    }
    // Put compiled arg on args block
    Ber args_block_ptr = local_get_int32(module, args_block_ptr_local);
    children[children_count++] =
        BinaryenStore(module, 4, offset, 2, args_block_ptr, compiled_arg,
                      BinaryenTypeInt32());
    // And loop
    offset += 4;
    args = args->cdr;
  }

  NativeFn* native_fn =
      alist_get(wasm->wajure_to_native_fn_map, is_eq_str, "partial");
  Ber partial_fn_index = make_int32(module, native_fn->fn_table_index);
  Ber ber_args_count = make_int32(module, args_count);
  args_block_ptr = local_get_int32(module, args_block_ptr_local);
  Ber partial_operands[] = {make_int32(module, 0), make_int32(module, 0),
                            args_block_ptr, ber_args_count};

  children[children_count++] =
      BinaryenCallIndirect(module, partial_fn_index, partial_operands, 4,
                           make_type_int32(4), BinaryenTypeInt32());

  Ber block = BinaryenBlock(module, NULL, children, children_count,
                            BinaryenTypeInt32());
  /* li_close(li); */
  CResult ret = {.ber = block, .is_fn_call = 1};
  return ret;
}

CResult compile_rt_partial_call(Wasm* wasm, CResult fn_arg, Cell* args,
                                int args_count) {
  BinaryenModuleRef module = wasm->module;
  printf("================================\n");
  lval_println(fn_arg.lval);
  // Put the compiled args in a block (lalloc_size) and pass them to our
  // native partial fn
  // Check for type:
  // 1. LVAL_WASM_LAMBDA -> add partials to a copy of the lval_wasm_lambda
  // of fn and args to partial, return it
  // 2.TODO: vector/set/map -> make lval_wasm_lambda of get and args to
  // partial fn as partials
  // 3. anything else: return as is

  int extra_children = 5;
  Ber* children = malloc((args_count + extra_children) * sizeof(Ber));
  int children_count = 0;
  int wval_local = li_new(wasm);
  children[children_count++] = BinaryenLocalSet(module, wval_local, fn_arg.ber);
  Ber wval = BinaryenLocalGet(module, wval_local, BinaryenTypeInt32());

  Ber ber_args_count = make_int32(module, args_count - 1);

  // Calculate new partial count (partial count of fn plus count of args)
  int partial_count_local = li_new(wasm);
  Ber wval_partial_count_tee = BinaryenLocalTee(
      module, partial_count_local, get_wval_prop(module, wval, "partial_count"),
      BinaryenTypeInt32());
  Ber new_partial_count = BinaryenBinary(
      module, BinaryenAddInt32(), wval_partial_count_tee, ber_args_count);
  int new_partial_count_local = li_new(wasm);
  Ber new_partial_count_tee = BinaryenLocalTee(
      module, new_partial_count_local, new_partial_count, BinaryenTypeInt32());

  // Reserve a block of memory for the partials
  Ber lalloc_size_operands[] = {BinaryenBinary(module, BinaryenMulInt32(),
                                               new_partial_count_tee,
                                               make_int32(module, 4))};
  Ber lalloc_new_partials = BinaryenCall(
      module, "lalloc_size", lalloc_size_operands, 1, BinaryenTypeInt32());
  int new_partials_local = li_new(wasm);
  Ber set_new_partials_local =
      BinaryenLocalSet(module, new_partials_local, lalloc_new_partials);
  children[children_count++] = set_new_partials_local;

  // Copy partials of fn (first arg of partial) into this block, retaining
  // them
  Ber copy_and_retain_operands[] = {
      get_wval_prop(module, wval, "partials"),       // partials of old fn
      local_get_int32(module, partial_count_local),  // old partial count
      local_get_int32(module,
                      new_partials_local)};  // ptr to partials of new fn

  NativeFn* native_fn =
      alist_get(wasm->wajure_to_native_fn_map, is_eq_str, "copy_and_retain");
  Ber copy_and_retain_fn_index = make_int32(module, native_fn->fn_table_index);
  Ber copy_wval_partials = BinaryenCallIndirect(
      module, copy_and_retain_fn_index, copy_and_retain_operands, 3,
      make_type_int32(3), BinaryenTypeInt32());

  int updated_partials_ptr_local = li_new(wasm);
  children[children_count++] =
      BinaryenLocalSet(module, updated_partials_ptr_local, copy_wval_partials);

  int offset = 0;
  // Copy rest of args into this block, retaining any non fn calls
  while (args) {
    CResult result = lval_compile(wasm, args->car);
    Ber compiled_arg = result.ber;

    // Any arg that's not the result of a fn call will be retained, since we
    // want to be able to release all partials when the fn is released
    if (!result.is_fn_call) wasm_retain(wasm, compiled_arg);

    // Put compiled arg on args block
    Ber partials_ptr = BinaryenLocalGet(module, updated_partials_ptr_local,
                                        BinaryenTypeInt32());
    children[children_count++] = BinaryenStore(
        module, 4, offset, 2, partials_ptr, compiled_arg, BinaryenTypeInt32());

    // And loop
    offset += 4;
    args = args->cdr;
  }

  // Copy props of fn to our partial fn
  Ber wval_fn_table_index = get_wval_prop(module, wval, "fn_table_index");
  Ber wval_closure =
      wasm_retain(wasm, get_wval_prop(module, wval, "closure")).ber;
  Ber wval_param_count = get_wval_prop(module, wval, "param_count");
  Ber wval_has_rest_arg = get_wval_prop(module, wval, "has_rest_arg");

  Ber operands[6] = {
      wval_fn_table_index,
      wval_param_count,
      wval_has_rest_arg,
      wval_closure,
      BinaryenLocalGet(module, new_partials_local, BinaryenTypeInt32()),
      BinaryenLocalGet(module, new_partial_count_local, BinaryenTypeInt32()),
  };

  // TODO: if fn is not an function we want to return just the value itself,
  // retained, since at compile time we don't know if it's been a real fn
  // call, so we'll pretend it was, and the result of the whole partial call
  // can then be released when required

  // Make the new wval_fn
  Ber make_lval_wasm_lambda_call = BinaryenCall(
      wasm->module, "make_lval_wasm_lambda", operands, 6, make_type_int32(1));
  children[children_count++] = make_lval_wasm_lambda_call;

  Ber block = BinaryenBlock(module, uniquify_name(wasm, "partial_call"),
                            children, children_count, BinaryenTypeInt32());

  CResult ret = {.ber = block, .is_fn_call = 1};

  return ret;
}

CResult compile_partial_call(Wasm* wasm, NativeFn native_fn, Cell* args) {
  BinaryenModuleRef module = wasm->module;
  int args_count = list_count(args);
  if (args_count == 0) quit(wasm, "Need at last one argument for partial");
  Cell* head = args;
  CResult fn_arg = lval_compile(wasm, head->car);

  head = head->cdr;
  // If just one arg, return compiled arg
  if (!head) {
    return fn_arg;
  }

  if (fn_arg.is_fn_call || fn_arg.lval->type == LVAL_REF) {
    /* return test_native_partial_fn(wasm, fn_arg, head, list_count(args)); */
    return compile_rt_partial_call(wasm, fn_arg, head, list_count(args));
  }

  switch (fn_arg.lval->type) {
    case LVAL_FUNCTION: {
      Lval* lval_fn = fn_arg.lval;

      // If we know this is a function at compile time then we compile the
      // rest of the args and put them in the partials block of a copy of the
      // lval_wasm_lambda we know we have.

      int fn_partial_count = list_count(lval_fn->partials);
      int total_partial_count = (fn_partial_count + (args_count - 1));

      // set_local_to_partials_ptr and make_partial_fn call
      int extra_children = 2;
      Ber* children =
          malloc(sizeof(Ber) * (total_partial_count + extra_children));
      int children_count = 0;

      // Allocate a block for the partials (including any partials of the fn
      // itself)
      Ber lalloc_size_operands[] = {
          make_int32(module, total_partial_count * 4)};
      Ber lalloc_partials = BinaryenCall(
          module, "lalloc_size", lalloc_size_operands, 1, BinaryenTypeInt32());
      int partials_ptr_index = li_new(wasm);
      Ber set_local_to_partials_ptr =
          BinaryenLocalSet(module, partials_ptr_index, lalloc_partials);
      children[children_count++] = set_local_to_partials_ptr;

      int partial_count = 0;

      // Add any partials that the fn has already
      Cell* partials_head = lval_fn->partials;
      while (partials_head) {
        Lval* lval = partials_head->car;
        CResult result = datafy_lval(wasm, lval);
        Ber wval_ptr = make_int32(module, result.wasm_ptr);
        wval_ptr = BinaryenBinary(
            module, BinaryenAddInt32(), wval_ptr,
            BinaryenGlobalGet(module, "data_offset", BinaryenTypeInt32()));
        wval_ptr = wasm_retain(wasm, wval_ptr).ber;
        Ber partials_ptr =
            BinaryenLocalGet(module, partials_ptr_index, BinaryenTypeInt32());
        children[children_count++] =
            BinaryenStore(module, 4, partial_count++ * 4, 2, partials_ptr,
                          wval_ptr, BinaryenTypeInt32());
        partials_head = partials_head->cdr;
      }

      // Compile the args and put their lval_refs on the partials block. If
      // they're not the result of a fn call retain them.
      do {
        CResult compiled_arg = lval_compile(wasm, head->car);
        // This is so that we can release all partials when releasing the
        // lval_wasm_lambda:
        if (!compiled_arg.is_fn_call)
          compiled_arg.ber = wasm_retain(wasm, compiled_arg.ber).ber;
        Ber partials_ptr =
            BinaryenLocalGet(module, partials_ptr_index, BinaryenTypeInt32());
        children[children_count++] =
            BinaryenStore(module, 4, partial_count++ * 4, 2, partials_ptr,
                          compiled_arg.ber, BinaryenTypeInt32());
        head = head->cdr;
      } while (head);

      // Copy the props of the LVAL_FUN to our new lval_wasm_lambda:
      Ber closure_ptr = make_int32(module, 0);
      Ber partials_ptr =
          BinaryenLocalGet(module, partials_ptr_index, BinaryenTypeInt32());
      Ber ber_partial_count = make_int32(module, partial_count);

      int fn_table_index =
          lval_fn->cfn ? lval_fn->cfn->offset : lval_fn->offset;
      Ber ber_fn_table_index;
      Ber param_count;
      Ber has_rest_arg;
      if (lval_fn->subtype == SYS) {
        // Let's get fn_table_index from the datafied sys fn
        ber_fn_table_index =
            get_wval_prop(module, fn_arg.ber, "fn_table_index");
        param_count = make_int32(module, 1);
        has_rest_arg = make_int32(module, 1);
      } else {
        // We know the offset/fn_table_index, let's add the module's
        // fn_table_offset
        ber_fn_table_index = make_int32(module, fn_table_index);
        ber_fn_table_index = BinaryenBinary(
            module, BinaryenAddInt32(), ber_fn_table_index,
            BinaryenGlobalGet(module, "fn_table_offset", BinaryenTypeInt32()));
        param_count = make_int32(module, lval_fn->param_count);
        has_rest_arg = make_int32(module, lval_fn->rest_arg_index);
      }

      // Call the runtime make_lval_wasm_lambda fn
      Ber operands[6] = {ber_fn_table_index, param_count,  has_rest_arg,
                         closure_ptr,        partials_ptr, ber_partial_count};
      children[children_count++] =
          BinaryenCall(wasm->module, "make_lval_wasm_lambda", operands, 6,
                       make_type_int32(1));

      // And return a block that does all of the above
      Ber block = BinaryenBlock(module, uniquify_name(wasm, "make_partial_fn"),
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
      // of the args still for side effects, ie we only compile fn calls.
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
      children[children_count++] = fn_arg.ber;

      Ber block = BinaryenBlock(
          module, uniquify_name(wasm, "make_pointless_partial_fn"), children,
          children_count, BinaryenTypeInt32());
      free(children);
      return cresult(block);
    }
  }
}

CResult compile_native_call(Wasm* wasm, Lval* lval_fn_native, Cell* args) {
  NativeFn* native_fn =
      alist_get(wasm->wajure_to_native_fn_map, is_eq_str, lval_fn_native->str);
  if (!native_fn)
    quit(wasm, "Function %s not found in runtime", lval_fn_native->str);
  return native_fn->compile_fn_call(wasm, *native_fn, args);
}
