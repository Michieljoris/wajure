#ifndef __ENV_H_
#define __ENV_H_

#include <stdbool.h>

#include "lval.h"

enum { ROOT_ENV, LOCAL_ENV };
enum { BUILTIN, USER_DEFINED };

Lenv* lenv_new(void);
void lenv_del(Lenv* e);
Lenv* make_lenv_copy(Lenv* env);
Lval* lenv_get(Lenv* e, Lval* k);
bool lenv_put(Lenv* e, Lval* k, Lval* v, int is_builtin);
Lenv* get_root_env(Lenv* env);

#endif  // __ENV_H_
