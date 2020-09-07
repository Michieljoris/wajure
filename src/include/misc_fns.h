#ifndef __MISC_FNS_H_
#define __MISC_FNS_H_

#include "env.h"

char* read_file(char* file_name);
Lval* load(Lenv* _, char* file_name);
extern int exit_repl;

Lval* get_lval_ns(Lenv* env);

Lval* require_ns(Lenv* _, char* namespace);

Lval* in_ns(char* namespace_str);

Lval* get_lval_ns(Lenv* env);

#endif  // __MISC_FNS_H_
