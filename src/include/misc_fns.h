#ifndef __MISC_FNS_H_
#define __MISC_FNS_H_

#include "env.h"

Lval* slurp(Lenv* env, char* file_name);
Builtin misc_builtins[11];
int exit_repl;

#endif  // __MISC_FNS_H_
