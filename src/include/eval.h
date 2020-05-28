#ifndef __EVAL_H_
#define __EVAL_H_

#include "env.h"
#include "lval.h"

Lval* lval_eval(Lenv* e, Lval* v);
Lval* eval_nodes(Lenv* env, Lval* lval, int count);
Lval* eval_lambda_call(Lenv* env, Lval* lval_fun, Lval* sexpr_args);
#endif  // __EVAL_H_
