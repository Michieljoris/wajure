#ifndef __NATIVE_H_
#define __NATIVE_H_

#include "wasm.h"

void add_call_fns(Wasm* wasm);
void add_validate_fn_fn(Wasm* wasm);
void add_copy_and_retain_fn(Wasm* wasm);
void add_partial_fn(Wasm* wasm);
void add_apply_fn(Wasm* wasm);

#endif  // __NATIVE_H_
