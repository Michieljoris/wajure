#ifndef __COMPILE_H_
#define __COMPILE_H_

#include "wasm.h"
int compile(char* file_name);

Ber lval_compile(Wasm* wasm, Lval* lval);

#endif  // __COMPILE_H_
