#ifndef __INTER_H_
#define __INTER_H_

#include "ltypes.h"
#include "wasm.h"

CResult inter_lval(Wasm* wasm, Lval* lval);
CResult inter_cell(Wasm* wasm, Cell* cell);

CResult inter_list(Wasm* wasm, Lval* lval);

int* make_data_lval_wasm_lambda(Wasm* wasm, int fn_table_index, int param_count,
                                int has_rest_arg);
CResult inter_data_lval_wasm_lambda(Wasm* wasm, int* data_lval);

#endif  // __INTER_H_
