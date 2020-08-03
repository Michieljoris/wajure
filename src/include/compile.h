#ifndef __COMPILE_H_
#define __COMPILE_H_

#include "wasm.h"
int compile(char* file_name);

Ber lval_compile(Wasm* wasm, Lval* lval);

Ber compile_do_list(Wasm* wasm, Lval* list, int mode);

#endif  // __COMPILE_H_
