#ifndef __COMPILE_SPECIAL_H_
#define __COMPILE_SPECIAL_H_

#include "wasm.h"

Ber compile_let(Wasm* wasm, Cell* args);
Ber compile_if(Wasm* wasm, Cell* args);
Ber compile_try(Wasm* wasm, Cell* args);
Ber compile_throw(Wasm* wasm, Cell* args);
Ber compile_quote(Wasm* wasm, Cell* args);
Ber compile_quasiquote(Wasm* wasm, Cell* args);

#endif  // __COMPILE_SPECIAL_H_
