#ifndef __COMPILE_SPECIAL_H_
#define __COMPILE_SPECIAL_H_

#include "wasm.h"

CResult compile_let(Wasm* wasm, Cell* args);
CResult compile_if(Wasm* wasm, Cell* args);
CResult compile_try(Wasm* wasm, Cell* args);
CResult compile_throw(Wasm* wasm, Cell* args);
CResult compile_quote(Wasm* wasm, Cell* args);
CResult compile_quasiquote(Wasm* wasm, Cell* args);

#endif  // __COMPILE_SPECIAL_H_
