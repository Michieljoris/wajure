#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "lval.h"
#include "print.h"

lval* lval_eval(lenv* e, lval* v);

static lval* lval_eval_sexpr(lenv* e, lval* sexpr) {
  /* eval children */
  for (int i = 0; i < sexpr->count; i++) {
    sexpr->cell[i] = lval_eval(e, sexpr->cell[i]);
  }
  /* error checking */
  for (int i = 0; i < sexpr->count; i++) {
    if (sexpr->cell[i]->type == LVAL_ERR) {
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
  lval* fun = lval_pop(sexpr, 0);
  if (fun->type != LVAL_FUN) {
    lval_del(fun);
    lval_del(sexpr);
    return lval_err("sexpr doesn't start with a function");
  }

  /* sexpr has all elements now except for first (a LVAL_FUN) */
  lval* result = fun->fun(e, sexpr);
  lval_del(fun);
  return result;
}

lval* lval_eval(lenv* env, lval* value) {
  if (value->type == LVAL_SYM) {
    lval* resolved_symbol = lenv_get(env, value);

    lval_del(value);
    return resolved_symbol;
  }
  if (value->type == LVAL_SEXPR) {
    return lval_eval_sexpr(env, value);
  }
  return value;
}
