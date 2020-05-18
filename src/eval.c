#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "lval.h"
#include "print.h"

Lval* lval_eval(Lenv* e, Lval* v);

static Lval* lval_eval_sexpr(Lenv* e, Lval* sexpr) {
  /* eval children */
  for (int i = 0; i < sexpr->count; i++) {
    sexpr->node[i] = lval_eval(e, sexpr->node[i]);
  }
  /* error checking */
  for (int i = 0; i < sexpr->count; i++) {
    if (sexpr->node[i]->type == LVAL_ERR) {
      return lval_take(sexpr, i);
    }
  }
  /* empty expr */
  if (sexpr->count == 0) {
    return sexpr;
  }
  /* single expr */
  /* if (v->count == 1) { */
  /*   return lval_take(v, 0); */
  /* } */

  /* first expr should have evalled to a fun*/
  Lval* lval_fun = lval_pop(sexpr, 0);
  if (lval_fun->type != LVAL_FUN) {
    lval_del(lval_fun);
    lval_del(sexpr);
    return make_lval_err("sexpr doesn't start with a function");
  }

  /* sexpr has all elements now except for first (a LVAL_FUN) */
  Lval* lval = lval_fun->fun(e, sexpr);
  lval_del(lval_fun);
  return lval;
}

Lval* lval_eval(Lenv* env, Lval* lval) {
  if (lval->type == LVAL_SYM) {
    Lval* lval_resolved_sym = lenv_get(env, lval);

    lval_del(lval);
    return lval_resolved_sym;
  }
  if (lval->type == LVAL_SEXPR) {
    return lval_eval_sexpr(env, lval);
  }
  return lval;
}
