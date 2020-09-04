#ifndef __RUNTIME_H_
#define __RUNTIME_H_

#include "ltypes.h"

WvalFun* make_lval_wasm_lambda(int fn_table_index, int param_count,
                               int has_rest_arg, int closure, int partials,
                               int partial_count);

void init_rest_args(Lval** lval_array, int rest_arg_length);

void wval_print(WvalFun* wval);

int check_args_count(int param_count, int args_count, int has_rest_arg);

void rewrite_pointers(int data_start, int data_size, int fn_table_offset);

/* int is_falsy(Lval* lval); */
#endif  // __RUNTIME_H_
