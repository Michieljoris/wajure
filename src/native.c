#include "native.h"

#include "builtin.h"
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

void add_call_fn(Wasm* wasm, int n) {
  BinaryenModuleRef module = wasm->module;
  char* fn_name = number_fn_name(wasm, "call");  // call#0 - call#n
  add_fn_to_table(wasm, fn_name);

  // wval, args_block_ptr and args count
  int wval_param = 0;
  int args_block_ptr_param = 1;
  /* int args_count_param = 2; */

  int body_children_count = 0;
  Ber* body_children = malloc(10 * sizeof(Ber));  // TODO: 10 ????

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

  // We know the rest arg index (n-1), let's bundle any excess args
  Ber bundle_operands[] = {make_int32(module, n - 1),
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

void add_rt_error_fn(Wasm* wasm, int rt_error_code, char* fn_name,
                     char* error_msg) {
  BinaryenModuleRef module = wasm->module;

  int body_children_count = 0;
  Ber* body_children = malloc(2 * sizeof(Ber));
  scoped char* rt_error_msg = lalloc_size(_strlen(error_msg));
  sprintf(rt_error_msg, "%s", error_msg);

  Ber rt_error = wasm_runtime_error(wasm, rt_error_code, rt_error_msg).ber;

  body_children[body_children_count++] = rt_error;
  body_children[body_children_count++] = make_int32(module, 0);
  Ber body = BinaryenBlock(module, NULL, body_children, body_children_count,
                           BinaryenTypeInt32());
  // Add function to wasm
  BinaryenType fn_params_type = make_type_int32(3);
  BinaryenType fn_results_type = make_type_int32(1);
  BinaryenAddFunction(module, fn_name, fn_params_type, fn_results_type, NULL, 0,
                      body);
}

void add_rt_error_too_few_args_fn(Wasm* wasm, char* fn_name) {
  int rt_error_code = RT_TOO_FEW_ARGS;
  char* error_msg = "fn";
  add_rt_error_fn(wasm, rt_error_code, fn_name, error_msg);
}

void add_rt_error_too_many_args_fn(Wasm* wasm, char* fn_name) {
  int rt_error_code = RT_NOT_A_FN;
  char* error_msg = "";
  add_rt_error_fn(wasm, rt_error_code, fn_name, error_msg);
}

void add_fn_rt_error_not_a_fn(Wasm* wasm, char* fn_name) {
  int rt_error_code = RT_TOO_MANY_ARGS;
  char* error_msg = "fn";
  add_rt_error_fn(wasm, rt_error_code, fn_name, error_msg);
}

void add_copy_and_retain_fn(Wasm* wasm, char* fn_name) {
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

  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, BinaryenLoop(module, loop, body));
}

// Put the compiled args in a block (lalloc_size) and pass them to our
// native partial fn
// Check for type:
// 1. LVAL_WASM_LAMBDA -> add partials to a copy of the lval_wasm_lambda
// of fn and args to partial, return it
// 2.TODO: vector/set/map -> make lval_wasm_lambda of get and args to
// partial fn as partials
// 3. anything else: return as is

void add_partial_fn(Wasm* wasm, char* fn_name) {
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

  WasmFn* native_fn =
      alist_get(state->wajure_to_native_fn_map, is_eq_str, "copy_and_retain");
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

  Ber operands[4] = {
      wval_fn_table_index,
      wval_closure,
      local_get_int32(module, partials_local),
      local_get_int32(module, total_partial_count_local),
  };

  // Make the new wval_fn
  Ber make_lval_wasm_lambda_call = BinaryenCall(
      wasm->module, "make_lval_wasm_lambda", operands, 4, make_type_int32(1));
  children[children_count++] = make_lval_wasm_lambda_call;
  Ber block = BinaryenBlock(module, NULL, children, children_count,
                            BinaryenTypeInt32());

  // And add the partial fn to wasm
  BinaryenType params_type = make_type_int32(wasm_params_count);
  BinaryenType results_type = make_type_int32(1);

  int locals_count = context->function_context->local_count - wasm_params_count;
  BinaryenType* local_types = make_type_int32_array(locals_count);

  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, block);
}

void add_apply_fn(Wasm* wasm, char* fn_name) {
  BinaryenModuleRef module = wasm->module;
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(0);
  Ber body = BinaryenNop(module);
  BinaryenAddFunction(module, fn_name, params_type, results_type, NULL, 0,
                      body);
}
void add_keyword_fn(Wasm* wasm, char* fn_name) {
  BinaryenModuleRef module = wasm->module;
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(0);
  Ber body = BinaryenNop(module);
  BinaryenAddFunction(module, fn_name, params_type, results_type, NULL, 0,
                      body);
}

void add_symbol_fn(Wasm* wasm, char* fn_name) {
  BinaryenModuleRef module = wasm->module;
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(0);
  Ber body = BinaryenNop(module);
  BinaryenAddFunction(module, fn_name, params_type, results_type, NULL, 0,
                      body);
}

void add_map_fn(Wasm* wasm, char* fn_name) {
  BinaryenModuleRef module = wasm->module;
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(0);
  Ber body = BinaryenNop(module);
  BinaryenAddFunction(module, fn_name, params_type, results_type, NULL, 0,
                      body);
}

void add_vector_fn(Wasm* wasm, char* fn_name) {
  BinaryenModuleRef module = wasm->module;
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(0);
  Ber body = BinaryenNop(module);
  BinaryenAddFunction(module, fn_name, params_type, results_type, NULL, 0,
                      body);
}

void add_set_fn(Wasm* wasm, char* fn_name) {
  BinaryenModuleRef module = wasm->module;
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(0);
  Ber body = BinaryenNop(module);
  BinaryenAddFunction(module, fn_name, params_type, results_type, NULL, 0,
                      body);
}

int get_fn_table_index(Wasm* wasm, char* fn_name) {
  WasmFn* native_fn =
      alist_get(state->wajure_to_native_fn_map, is_eq_str, fn_name);
  return native_fn->fn_table_index;
}
