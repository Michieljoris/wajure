#ifndef __EVAL_H_
#define __EVAL_H_

#include "env.h"
#include "lval.h"

Lval* lval_eval(Lenv* e, Lval* v);
Lval* map_eval(Lenv* env, Lval* lval);
enum { RETURN_ON_ERROR, RETURN_LAST_EVAL };
Lval* do_list(Lenv* env, Lval* list, int mode);
Lval* eval_lambda_call(Lval* lval_fun, Lval* arg_list);
Lval* expand_macro(Lval* lval_fun, Lval* arg_list);
struct resolved_symbol eval_symbol(Lenv* env, Lval* lval_symbol);
#endif  // __EVAL_H_
