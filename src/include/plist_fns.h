#ifndef __PLIST_FNS_H_
#define __PLIST_FNS_H_

#include "lval.h"
#include "mempool.h"

Cell* make_cell();
Lval* make_lval_plist();
void lenv_add_plist_fns(Lenv* env);

#endif  // __PLIST_FNS_H_
