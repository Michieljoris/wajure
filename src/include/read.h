#ifndef __READ_H_
#define __READ_H_

#include "lval.h"
#include "mpc.h"

/* Lval* lval_read(mpc_ast_t* t); */
Lval* read_expressions(Lval* lval, mpc_ast_t* expressions[],
                       int expression_count);

#endif  // __READ_H_
