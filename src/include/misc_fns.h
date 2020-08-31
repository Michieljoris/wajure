#ifndef __MISC_FNS_H_
#define __MISC_FNS_H_

#include "env.h"

char* read_file(char* file_name);
Lval* load(Lenv* env, char* file_name);
extern int exit_repl;

#endif  // __MISC_FNS_H_
