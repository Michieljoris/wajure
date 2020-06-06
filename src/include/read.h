#ifndef __READ_H_
#define __READ_H_

#include "lval.h"
#include "mpc.h"

Lval* read_list(Lval* list, mpc_ast_t* expression);

#endif  // __READ_H_
