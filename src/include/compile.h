#ifndef __COMPILE_H_
#define __COMPILE_H_

#include "wasm.h"
int compile(char* file_name);
CResult wasmify_literal(Wasm* wasm, Lval* lval);
CResult wasmify_collection(Wasm* wasm, Lval* lval);

CResult lval_compile(Wasm* wasm, Lval* lval);
CResult wasmify_lval(Wasm* wasm, Lval* lval);
#endif  // __COMPILE_H_
