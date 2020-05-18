#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "env.h"
#include "fns.h"
#include "lval.h"
#include "print.h"

Lval* lval_eval(Lenv* e, Lval* v);

Lval* lval_call(Lenv* env, Lval* lval_fun, Lval* sexpr) {
  if (lval_fun->fun) {
    return lval_fun->fun(env, sexpr);
  }
  for (int i = 0; i < sexpr->count; ++i) {
    bool is_not_internal_var =
        lenv_put(lval_fun->env, lval_fun->formals->node[i], sexpr->node[i],
                 USER_DEFINED);
    LASSERT(sexpr, is_not_internal_var,
            "Can't redefine internal variable '%s' ",
            lval_fun->formals->node[i]->sym)
  }
  lval_del(sexpr);

  lval_fun->env->parent_env = env;

  return eval_fn(lval_fun->env, lval_add_child(make_lval_sexpr(),
                                               make_lval_copy(lval_fun->body)));
}

static Lval* lval_eval_sexpr(Lenv* env, Lval* sexpr) {
  /* eval children */
  for (int i = 0; i < sexpr->count; i++) {
    sexpr->node[i] = lval_eval(env, sexpr->node[i]);
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
  if (sexpr->count == 1) {
    return lval_take(sexpr, 0);
  }

  /* first expr should have evalled to a fun*/
  Lval* lval_fun = lval_pop(sexpr, 0);
  if (lval_fun->type != LVAL_FUN) {
    lval_del(lval_fun);
    lval_del(sexpr);
    return make_lval_err("sexpr doesn't start with a function");
  }

  /* sexpr has all elements now except for first (a LVAL_FUN) */
  /* Lval* lval = lval_fun->fun(e, sexpr); */
  Lval* lval = lval_call(env, lval_fun, sexpr);
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
