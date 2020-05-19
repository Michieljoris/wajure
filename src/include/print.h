#ifndef __PRINT_H_
#define __PRINT_H_
#include "env.h"
#include "lval.h"

void lval_println(Lval* v);
void lval_print(Lval* lval);
void lenv_print(Lenv* e);

#endif  // __PRINT_H_
