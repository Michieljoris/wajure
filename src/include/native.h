#ifndef __NATIVE_H_
#define __NATIVE_H_

#include "wasm.h"

void add_call_fns(Wasm* wasm);
void add_bundle_rest_arg_fns(Wasm* wasm);
void add_rt_error_too_few_args_fn(Wasm* wasm);
void add_rt_error_too_many_args_fn(Wasm* wasm);
void add_validate_fn_fn(Wasm* wasm);
void add_copy_and_retain_fn(Wasm* wasm);
void add_partial_fn(Wasm* wasm);
void add_apply_fn(Wasm* wasm);

CResult compile_partial_call(Wasm* wasm, NativeFn native_fn, Cell* args);
CResult compile_native_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args);

#endif  // __NATIVE_H_
