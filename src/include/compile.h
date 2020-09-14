#ifndef __COMPILE_H_
#define __COMPILE_H_

#include "wasm.h"
void compile(Namespace* ns);

CResult lval_compile(Wasm* wasm, Lval* lval);

FunctionData add_wasm_function(Wasm* wasm, Lenv* env, char* fn_name,
                               Lval* lval_fun);

CResult wasm_process_args(Wasm* wasm, int param_count, int rest_arg_index);

int compile_main();
#endif  // __COMPILE_H_
