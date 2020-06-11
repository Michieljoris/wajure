#ifndef __LIST_FNS_H_
#define __LIST_FNS_H_

#include "env.h"

Lval* list_fn(Lenv* e, Lval* sexpr);
Builtin list_builtins[999];

#endif  // __LIST_FNS_H_
