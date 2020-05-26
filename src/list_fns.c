#include "list_fns.h"

#include "assert.h"
#include "env.h"
#include "eval.h"
#include "lval.h"

Lval* list_fn(Lenv* e, Lval* sexpr) { return sexpr; }

Lval* first_fn(Lenv* e, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "first");
  LASSERT_IS_LIST_TYPE(sexpr_args, 0, "first");

  Lval* lval = lval_take(sexpr_args, 0);
  return lval_take(lval, 0);
}

Lval* rest_fn(Lenv* e, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "rest");

  Lval* lval = lval_take(sexpr_args, 0);
  int type = lval->type;
  if (!(type == LVAL_SEXPR || type == LVAL_VECTOR || type == LVAL_MAP))
    return make_lval_err("first only works on list, vector or map, not a %s",
                         lval_type_to_name(type));

  lval_del(lval_pop(lval, 0));
  lval->type = LVAL_SEXPR;
  return lval;
}

Lval* concat_fn(Lenv* e, Lval* sexpr_args) {
  for (int i = 0; i < sexpr_args->count; i++) {
    LASSERT_IS_LIST_TYPE(sexpr_args, i, "concat");
  }
  Lval* lval = lval_pop(sexpr_args, 0);

  while (sexpr_args->count) {
    lval = lval_concat(lval, lval_pop(sexpr_args, 0));
  }

  lval_del(sexpr_args);
  return lval;
}

void lenv_add_list_fns(Lenv* env) {
  lenv_add_builtin(env, "list", list_fn);
  lenv_add_builtin(env, "first", first_fn);
  lenv_add_builtin(env, "rest", rest_fn);
  lenv_add_builtin(env, "concat", concat_fn);
}
