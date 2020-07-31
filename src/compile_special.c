#include "compile_special.h"

#include <binaryen-c.h>

#include "wasm.h"
#include "wasm_util.h"

Ber compile_let(Wasm* wasm, Cell* args) { return make_int32(wasm->module, 0); }

Ber compile_if(Wasm* wasm, Cell* args) { return make_int32(wasm->module, 0); }

Ber compile_try(Wasm* wasm, Cell* args) { return make_int32(wasm->module, 0); }

Ber compile_throw(Wasm* wasm, Cell* args) {
  return make_int32(wasm->module, 0);
}
Ber compile_quote(Wasm* wasm, Cell* args) {
  return make_int32(wasm->module, 0);
}
Ber compile_quasiquote(Wasm* wasm, Cell* args) {
  return make_int32(wasm->module, 0);
}
