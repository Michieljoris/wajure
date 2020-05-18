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

Lval* lval_call(Lenv* env, Lval* lval_fun, Lval* sexpr_args) {
  if (lval_fun->fun) {
    return lval_fun->fun(env, sexpr_args);
  }

  int given = sexpr_args->count;
  int total = lval_fun->formals->count;

  /* bind any args */
  while (sexpr_args->count) {
    if (lval_fun->formals->count == 0) {
      lval_del(sexpr_args);
      return make_lval_err(
          "Function passed too many args. Got %i, expected %i.", given, total);
    }

    Lval* sym = lval_pop(lval_fun->formals, 0);
    if (strcmp(sym->sym, "&") == 0) {
      if (lval_fun->formals->count != 1) {
        lval_del(sexpr_args);
        return make_lval_err(
            "Function format invalid. "
            "Symbol '&' not followed by single symbol.");
      }
      Lval* rest_sym = lval_pop(lval_fun->formals, 0);
      lenv_put(lval_fun->env, rest_sym, list_fn(env, sexpr_args));
      lval_del(sym);
      lval_del(rest_sym);
      break;
    }
    Lval* val = lval_pop(sexpr_args, 0);

    bool is_not_internal_var = lenv_put(lval_fun->env, sym, val);
    LASSERT(sexpr_args, is_not_internal_var,
            "Can't redefine internal variable '%s' ", sym)
    lval_del(sym);
    lval_del(val);
  }

  lval_del(sexpr_args);

  if (lval_fun->formals->count > 0 &&
      strcmp(lval_fun->formals->node[0]->sym, "&") == 0) {
    if (lval_fun->formals->count != 2) {
      return make_lval_err(
          "Function format invalid. "
          "Symbol '&' not followed by single symbol.");
    }
    lval_del(lval_pop(lval_fun->formals, 0));
    Lval* sym = lval_pop(lval_fun->formals, 0);
    Lval* val = make_lval_qexpr();
    lenv_put(lval_fun->env, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  if (lval_fun->formals->count == 0) {
    lval_fun->env->parent_env = env;
    return eval_fn(
        lval_fun->env,
        lval_add_child(make_lval_sexpr(), make_lval_copy(lval_fun->body)));
  }

  return make_lval_copy(lval_fun);
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
    return make_lval_err(
        "sexpr starts with incorrect type. "
        "Got %s, expected %s.",
        lval_type_to_name(lval_fun->type), lval_type_to_name(LVAL_FUN));
  }

  /* sexpr has all elements now except for first (a LVAL_FUN) */
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
