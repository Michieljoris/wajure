#ifndef __RUNTIME_H_
#define __RUNTIME_H_

#include "ltypes.h"

WvalFun* make_lval_wasm_lambda(int fn_table_index, int param_count,
                               int has_rest_arg, int closure, int partials,
                               int partial_count);

int get_wval_type(Lval* lval);
int get_wval_subtype(Lval* lval);
int get_wval_fn_table_index(WvalFun* wval);
int get_wval_param_count(WvalFun* wval);
int get_wval_has_rest_arg(WvalFun* wval);
int get_wval_rest_arg_index(WvalFun* wval);
int get_wval_closure(WvalFun* wval);
int get_wval_partials(WvalFun* wval);
int get_wval_partial_count(WvalFun* wval);

void bundle_rest_args(Lval** lval_array, int rest_arg_length);

void wval_print(WvalFun* wval);

int check_args_count(int param_count, int args_count, int has_rest_arg);

void rewrite_pointers(int data_start, int data_size, int fn_table_offset);

void dbg(Lval* lval);

/* int is_falsy(Lval* lval); */
#endif  // __RUNTIME_H_
