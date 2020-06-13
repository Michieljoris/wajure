#ifndef __ENV_H_
#define __ENV_H_

#include "lval.h"

enum { ROOT_ENV, LOCAL_ENV };

Lenv* lenv_new(void);
void lenv_del(Lenv* e);
Lval* lenv_get(Lenv* e, Lval* k);
void lenv_put(Lenv* env, Lval* lval_sym, Lval* lval);
Lenv* lenv_assoc(Lenv* env, Lval* lval_sym, Lval* lval);
Lenv* lenv_prepend(Lenv* env, Lval* lval_sym, Lval* lval);
int lenv_is_bound(Lenv* env, Lval* lval_sym);
Lenv* get_root_env(Lenv* env);
#endif  // __ENV_H_
