#ifndef __LIST_FNS_H_
#define __LIST_FNS_H_

#include "env.h"
Lval* list_fn(Lenv* e, Lval* sexpr);
void lenv_add_list_fns(Lenv* env);

#endif  // __LIST_FNS_H_
