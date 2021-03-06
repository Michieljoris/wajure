#ifndef __DATAFY_H_
#define __DATAFY_H_
#include "ltypes.h"
#include "wasm.h"

Ber make_ptr(Wasm* wasm, int ptr);
CResult datafy_lval(Wasm* wasm, Lval* lval);
CResult datafy_nil(Wasm* wasm);
CResult datafy_empty_list(Wasm* wasm, Lval* lval_list);

#endif  // __DATAFY_H_
