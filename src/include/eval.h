#ifndef __EVAL_H_
#define __EVAL_H_

#include "env.h"
#include "lval.h"

#define WITH_TCO true
#define WITHOUT_TCO false

Lval* lval_eval(Lenv* e, Lval* v);
Lval* eval_nodes(Lenv* env, Lval* lval, int count);
Lval* eval_list(Lenv* env, Lval* body, bool with_tco);
Lval* eval_macro_call(Lenv* env, Lval* lval_fun, Lval* sexpr_args,
                      int with_tco);
Lval* eval_lambda_call(Lval* lval_fun, Lval* sexpr_args, int with_tco);
#endif  // __EVAL_H_
