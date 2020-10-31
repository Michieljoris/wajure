#ifndef __RUNTIME_H_
#define __RUNTIME_H_

#include "ltypes.h"

Lval* make_lval_wasm_lambda(int fn_table_index, int closure, int partials,
                            int partial_count);

int get_wval_group(Lval* lval);
int get_wval_type(Lval* lval);
int get_wval_fn_table_index(Lval* wval);
/* int get_wval_param_count(Lval* wval); */
/* int get_wval_has_rest_arg(Lval* wval); */
/* int get_wval_rest_arg_index(Lval* wval); */
int get_wval_closure(Lval* wval);
int get_wval_partials(Lval* wval);
int get_wval_partial_count(Lval* wval);

void bundle_rest_args(int rest_arg_index, Lval** args, int args_count);
Lval* listify_args(Lval** args, int args_count);

void wval_print(Lval* wval);

void rewrite_pointers(int data_start, int data_size, int fn_table_offset);

void dbg(Lval* lval);

/* int is_falsy(Lval* lval); */
#endif  // __RUNTIME_H_
