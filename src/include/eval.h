#ifndef __EVAL_H_
#define __EVAL_H_

#include "env.h"
#include "lval.h"
lval* lval_eval(lenv* e, lval* v);

#endif  // __EVAL_H_
