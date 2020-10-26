#ifndef __MAKE_WASM_FN_H_
#define __MAKE_WASM_FN_H_

#include "ltypes.h"
#include "wasm.h"

Ber switch_on_args_count(Wasm* wasm, Lval* lval_fn, int args_count_param,
                         LambdaBer get_lambda_ber);

enum { ABI_ARGS_BLOCK, ABI_PARAMS };

FunctionData add_wasm_function(Wasm* wasm, Lenv* env, char* fn_name,
                               Lambda* lambda, int wajure_args);

char* make_wajure_fn_name(Lval* lval_fn, int index);
int add_root_fn(Wasm* wasm, Lval* lval_fn);
int add_local_lambda(Wasm* wasm, Lval* lval_fn);
#endif  // __MAKE_WASM_FN_H_
