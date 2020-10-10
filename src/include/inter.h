#ifndef __INTER_H_
#define __INTER_H_

#include "ltypes.h"
#include "wasm.h"

int inter_lval(Wasm* wasm, Lval* lval);
/* int inter_cell(Wasm* wasm, Cell* cell); */

int inter_list(Wasm* wasm, Lval* lval);

char* make_data_lval_wasm_lambda(Wasm* wasm, int fn_table_index,
                                 int param_count, int has_rest_arg,
                                 Cell* partials);
int inter_data_lval_wasm_lambda(Wasm* wasm, char* data_lval);

int inter_lval_str_type(Wasm* wasm, Cell** pool, Lval* lval);

void inter_rewrite_info(Wasm* wasm);

#endif  // __INTER_H_
