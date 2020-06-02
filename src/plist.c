#include "plist.h"

#include <stdio.h>

#include "assert.h"
#include "lval.h"
#include "mempool.h"
#include "plist.h"

Lval* make_lval_plist(Mempool* mempool) {
  Lval* lval = mempool_alloc(mempool);
  lval->type = LVAL_SEQ;
  lval->subtype = PLIST;
  lval->count = 0;
  lval->cdr = NULL;
  lval->tco_env = NULL;
  return lval;
}

Lval* cons_fn(Mempool* mempool, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 2, "cons");
  LASSERT_NODE_TYPE(sexpr_args, 1, LVAL_SEQ, "cons");
  LASSERT_NODE_SUBTYPE(sexpr_args, 1, LIST, "cons");
  Lval* lval = make_lval_copy(sexpr_args->node[0]);
  lval->cdr = sexpr_args->node[1];
  lval_del(sexpr_args);
  return lval;
}

/* Lval* nth_fn(Mempool* mempool, Lval* sexpr_args) { */
/*   LASSERT_NODE_COUNT(sexpr_args, 2, "macroexpand"); */
/*   LASSERT_NODE_TYPE(sexpr_args, 1, LVAL_SEQ, "def"); */
/*   LASSERT_NODE_SUBTYPE(sexpr_args, 1, LIST, "def"); */
/* } */
