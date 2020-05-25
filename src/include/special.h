#ifndef __SPECIAL_H_
#define __SPECIAL_H_

#include "lval.h"
int lookup_special_sym(char* sym);
Lval* eval_special(Lenv* env, long special_sym, Lval* sexpr);
#endif  // __SPECIAL_H_
