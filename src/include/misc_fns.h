#ifndef __MISC_FNS_H_
#define __MISC_FNS_H_

#include "env.h"
Lval* load_fn(Lenv* env, Lval* sexpr_args);
Lval* mpc_load_fn(Lenv* env, Lval* arg_list);
void lenv_add_misc_fns(Lenv* env);
int exit_repl;

#endif  // __MISC_FNS_H_
