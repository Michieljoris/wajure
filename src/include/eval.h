#ifndef __EVAL_H_
#define __EVAL_H_

#include "env.h"
#include "lval.h"

#define EVAL_ALL_BUT_LAST 1
#define EVAL_ALL 0

Lval* lval_eval(Lenv* e, Lval* v);
Lval* eval_nodes(Lenv* env, Lval* lval);
Lval* eval_body(Lenv* env, Lval* body, int with_tco);
Lval* eval_lambda_call(Lval* lval_fun, Lval* arg_list, int with_tco);
#endif  // __EVAL_H_
