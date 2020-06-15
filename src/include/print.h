#ifndef __PRINT_H_
#define __PRINT_H_
#include "env.h"
#include "lval.h"

void lval_debugln(Lval* v);
void lval_infoln(Lval* v);
void lval_debug(Lval* lval);
void lval_pr(Lval* lval);
void lenv_print(Lenv* e);
void alist_print(Cell* alist);
void env_print(Lenv* env);

#endif  // __PRINT_H_
