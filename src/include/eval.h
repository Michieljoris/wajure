#ifndef __EVAL_H_
#define __EVAL_H_

#include "env.h"
#include "lval.h"

Lval* lval_eval(Lenv* e, Lval* v);
Lval* map_eval(Lenv* env, Lval* lval);
Lval* do_list(Lenv* env, Lval* list);
/* Lval* eval_list_but_last(Lenv* env, Lval* body); */
Lval* eval_lambda_call(Lval* lval_fun, Lval* arg_list);
Lval* expand_macro(Lval* lval_fun, Lval* arg_list);
Lval* eval_string(Lenv* env, char* str);
#endif  // __EVAL_H_
