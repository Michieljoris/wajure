#ifndef __MISC_FNS_H_
#define __MISC_FNS_H_

#include "env.h"
Lval* load_fn(Lenv* env, Lval* sexpr_args);
void lenv_add_misc_fns(Lenv* env);
bool exit_repl;

#endif  // __MISC_FNS_H_
