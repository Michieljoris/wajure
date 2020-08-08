#ifndef __LVAL_H_
#define __LVAL_H_
#include "ltypes.h"

Lval* make_lval_num(long x);
Lval* make_lval_quote(void);
Lval* make_lval_sym(char* s);
Lval* make_lval_str(char* s);
Lval* make_lval_list(void);
Lval* make_lval_map(void);
Lval* make_lval_vector(void);
Lval* make_lval_fun(Lbuiltin func, char* func_name, int type);
Lval* make_lval_lambda(Lenv* env, Lval* formals, Lval* body, int subtype);
Lval* make_lval_macro(Lval* formals, Lval* body);
Lval* make_lval_err(char* fmt, ...);
Lval* make_lval_exception(char* msg);
Lval* make_lval_nil(void);
Lval* make_lval_true(void);
Lval* make_lval_false(void);
Lval* make_lval_wasm_lambda(int fn_table_index, int param_count,
                            int has_rest_arg, Lval** closure, Lval** partials,
                            int partial_count);

char* lval_type_constant_to_name(int t);
char* lval_type_to_name(Lval* lval);

Lval* new_lval_list(void* head);

#endif  // __LVAL_H_
