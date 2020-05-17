#ifndef __ENV_H_
#define __ENV_H_

#include <stdbool.h>

#include "lval.h"

Lenv* lenv_new(void);
void lenv_del(Lenv* e);
Lval* lenv_get(Lenv* e, Lval* k);
bool lenv_put(Lenv* e, Lval* k, Lval* v, bool is_builtin);

#endif  // __ENV_H_
