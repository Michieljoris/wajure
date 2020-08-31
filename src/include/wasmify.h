#ifndef __WASMIFY_H_
#define __WASMIFY_H_
#include "ltypes.h"
#include "wasm.h"

CResult wasmify_literal(Wasm* wasm, Lval* lval);
CResult wasmify_collection(Wasm* wasm, Lval* lval);
CResult wasmify_lval(Wasm* wasm, Lval* lval);

#endif  // __WASMIFY_H_
