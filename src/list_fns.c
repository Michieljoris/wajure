#include "list_fns.h"

#include "assert.h"
#include "env.h"
#include "eval.h"
#include "lval.h"

Lval* head_fn(Lenv* e, Lval* sexpr) {
  LASSERT_CELL_COUNT(sexpr, 1, "head");
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, "head");

  Lval* qexpr = lval_take(sexpr, 0);
  while (qexpr->count > 1) {
    lval_del(lval_pop(qexpr, 1));
  }
  return qexpr;
}

Lval* tail_fn(Lenv* e, Lval* sexpr) {
  LASSERT_CELL_COUNT(sexpr, 1, "tail");
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, "tail");
  Lval* qexpr = sexpr->node[0];
  LASSERT(sexpr, qexpr->count != 0, "Function 'tail_fn' passed {}");

  qexpr = lval_take(sexpr, 0);
  lval_del(lval_pop(qexpr, 0));
  return qexpr;
}

Lval* list_fn(Lenv* e, Lval* sexpr) {
  sexpr->type = LVAL_QEXPR;
  return sexpr;
}

Lval* lval_join(Lval* x, Lval* y) {
  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add_child(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  lval_del(y);
  return x;
}

Lval* join_fn(Lenv* e, Lval* sexpr) {
  for (int i = 0; i < sexpr->count; i++) {
    LASSERT_CELL_TYPE(sexpr, i, LVAL_QEXPR, "join");
  }

  Lval* qexpr = lval_pop(sexpr, 0);

  while (sexpr->count) {
    qexpr = lval_join(qexpr, lval_pop(sexpr, 0));
  }

  lval_del(sexpr);
  return qexpr;
}

Lval* eval_fn(Lenv* env, Lval* sexpr) {
  LASSERT_CELL_COUNT(sexpr, 1, "eval");
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, "eval");
  Lval* qexpr = lval_take(sexpr, 0);
  qexpr->type = LVAL_SEXPR;
  return lval_eval(env, qexpr);
}

void lenv_add_list_fns(Lenv* env) {
  lenv_add_builtin(env, "list", list_fn);
  lenv_add_builtin(env, "head", head_fn);
  lenv_add_builtin(env, "tail", tail_fn);
  lenv_add_builtin(env, "eval", eval_fn);
  lenv_add_builtin(env, "join", join_fn);
}
