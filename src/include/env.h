#ifndef __ENV_H_
#define __ENV_H_

#include <stdbool.h>

#include "lval.h"

lenv* lenv_new(void);
void lenv_del(lenv* e);
lval* lenv_get(lenv* e, lval* k);
bool lenv_put(lenv* e, lval* k, lval* v, bool is_builtin);

#endif  // __ENV_H_
