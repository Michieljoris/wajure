#ifndef __DATAFY_H_
#define __DATAFY_H_
#include "ltypes.h"
#include "wasm.h"

Ber make_ptr(Wasm* wasm, int ptr);
CResult datafy_lval(Wasm* wasm, Lval* lval);
CResult datafy_nil(Wasm* wasm);
CResult datafy_empty_list(Wasm* wasm, Lval* lval_list);
char* make_wajure_fn_name(Lval* lval_fn);

#endif  // __DATAFY_H_
