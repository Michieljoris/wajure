#ifndef __PRINT_H_
#define __PRINT_H_
#include "env.h"
#include "lval.h"

void lval_println(lval* v);
void lenv_print(lenv* e);

#endif  // __PRINT_H_
