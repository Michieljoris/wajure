#ifndef __DATAFY_H_
#define __DATAFY_H_
#include "ltypes.h"
#include "wasm.h"

CResult datafy_literal(Wasm* wasm, Lval* lval);
CResult datafy_collection(Wasm* wasm, Lval* lval);
CResult datafy_lval(Wasm* wasm, Lval* lval);
CResult datafy_nil(Wasm* wasm);

#endif  // __DATAFY_H_
