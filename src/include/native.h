#ifndef __NATIVE_H_
#define __NATIVE_H_

#include "wasm.h"

void add_call_fns(Wasm* wasm);
void add_bundle_rest_arg_fns(Wasm* wasm);
void add_rt_error_too_few_args_fn(Wasm* wasm, char* fn_name);
void add_rt_error_too_many_args_fn(Wasm* wasm, char* fn_name);
void add_fn_rt_error_not_a_fn(Wasm* wasm, char* fn_name);
void add_validate_fn_fn(Wasm* wasm, char* fn_name);
void add_copy_and_retain_fn(Wasm* wasm, char* fn_name);
void add_partial_fn(Wasm* wasm, char* fn_name);
void add_apply_fn(Wasm* wasm, char* fn_name);

void add_keyword_fn(Wasm* wasm, char* fn_name);
void add_vector_fn(Wasm* wasm, char* fn_name);
void add_set_fn(Wasm* wasm, char* fn_name);
void add_symbol_fn(Wasm* wasm, char* fn_name);
void add_map_fn(Wasm* wasm, char* fn_name);

#endif  // __NATIVE_H_
