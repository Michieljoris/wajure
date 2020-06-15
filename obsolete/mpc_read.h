#ifndef __MPC_READ_H_
#define __MPC_READ_H_

#include "lval.h"
#include "mpc.h"

Lval* read_list(Lval* list, mpc_ast_t* expression);

#endif  // __MPC_READ_H_
