#ifndef __ENV_H_
#define __ENV_H_

#include <stdbool.h>

#include "lval.h"

enum { ROOT_ENV, LOCAL_ENV };

Lenv* lenv_new(void);
void lenv_del(Lenv* e);
Lenv* make_lenv_copy(Lenv* env);
Lval* lenv_get(Lenv* e, Lval* k);
Lenv* lenv_put(Lenv* env, Lval* lval_sym, Lval* lval);
int lenv_is_bound(Lenv* env, Lval* lval_sym);
Lenv* get_root_env(Lenv* env);
void lenv_add_builtin(Lenv* env, char* name, lbuiltin func, int subtype);
#endif  // __ENV_H_
