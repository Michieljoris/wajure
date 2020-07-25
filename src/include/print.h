#ifndef __PRINT_H_
#define __PRINT_H_
#include "env.h"

void lval_debugln(Lval* v);
void lval_infoln(Lval* v);
void lval_println(Lval* v);
void lval_print(Lval* v);
void lval_debug(Lval* lval);
void lval_pr(Lval* lval);
void lenv_print(Lenv* e);
void alist_print(Cell* alist);
void env_print(Lenv* env);

void print_kv(void* pair);

#endif  // __PRINT_H_
