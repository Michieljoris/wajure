#ifndef __COMPILE_PARTIAL_H_
#define __COMPILE_PARTIAL_H_

#include "wasm.h"

CResult compile_partial_call(Wasm* wasm, WasmFn native_fn, Cell* args);

#endif  // __COMPILE_PARTIAL_H_
