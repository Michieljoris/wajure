#ifndef __EVAL_H_
#define __EVAL_H_

#include "env.h"
#include "lval.h"

Lval* lval_eval(Lenv* e, Lval* v);
#endif  // __EVAL_H_
