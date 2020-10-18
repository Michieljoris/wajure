#ifndef __NATIVE_H_
#define __NATIVE_H_

#include "wasm.h"

// FCRA=Fn Call Relay Array
enum {
  FCRA_NOT_A_FN,
  FCRA_PARTIAL,
  FCRA_APPLY,
  FCRA_KEYWORD,
  FCRA_SYMBOL,
  FCRA_MAP,
  FCRA_VECTOR,
  FCRA_SET
};

#define FCRA_NOT_A_FN_OFFSET FCRA_NOT_A_FN * 21
#define FCRA_PARTIAL_OFFSET FCRA_PARTIAL * 21

void add_call_fns(Wasm* wasm);
void add_bundle_rest_arg_fns(Wasm* wasm);
void add_rt_error_too_few_args_fn(Wasm* wasm, char* fn_name);
void add_rt_error_too_many_args_fn(Wasm* wasm, char* fn_name);
void add_fn_rt_error_not_a_fn(Wasm* wasm, char* fn_name);
void add_validate_fn_fn(Wasm* wasm, char* fn_name);
void add_copy_and_retain_fn(Wasm* wasm, char* fn_name);
void add_partial_fn(Wasm* wasm, char* fn_name);
void add_apply_fn(Wasm* wasm, char* fn_name);

void add_keyword2_fn(Wasm* wasm, char* fn_name);
void add_keyword3_fn(Wasm* wasm, char* fn_name);
void add_symbol2_fn(Wasm* wasm, char* fn_name);
void add_symbol3_fn(Wasm* wasm, char* fn_name);
void add_map2_fn(Wasm* wasm, char* fn_name);
void add_map3_fn(Wasm* wasm, char* fn_name);
void add_vector_fn(Wasm* wasm, char* fn_name);
void add_set_fn(Wasm* wasm, char* fn_name);
void add_native_call_relay_arrays(Wasm* wasm);

#endif  // __NATIVE_H_
