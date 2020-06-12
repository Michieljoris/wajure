#ifndef __MISC_FNS_H_
#define __MISC_FNS_H_

#include "env.h"
Lval* load_fn(Lenv* env, Lval* arg_list);
/* Lval* mpc_load_fn(Lenv* env, Lval* arg_list); */
Builtin misc_builtins[10];
int exit_repl;

#endif  // __MISC_FNS_H_
