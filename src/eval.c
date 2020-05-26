#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "env.h"
#include "list_fns.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"
#include "special.h"

Lval* lval_eval(Lenv* e, Lval* v);

Lval* lval_call(Lenv* env, Lval* lval_fun, Lval* sexpr_args) {
  if (lval_fun->fun) return lval_fun->fun(env, sexpr_args);

  /* eval lispy fn */
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

    lenv_put(lval_fun->env, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  lval_del(sexpr_args);

  if (lval_fun->formals->count > 0 &&
      strcmp(lval_fun->formals->node[0]->sym, "&") == 0) {
    if (lval_fun->formals->count != 2)
      return make_lval_err(
          "Function format invalid. "
          "Symbol '&' not followed by single symbol.");

    lval_del(lval_pop(lval_fun->formals, 0));
    Lval* sym = lval_pop(lval_fun->formals, 0);
    Lval* val = make_lval_sexpr();
    lenv_put(lval_fun->env, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  /* eval body expressions */
  if (lval_fun->formals->count == 0) {
    lval_fun->env->parent_env = env;

    Lval* body = lval_fun->body;
    Lval* ret = NULL;

    while (body->count) {
      ret = lval_eval(lval_fun->env, lval_pop(body, 0));
      if (ret->type == LVAL_ERR) {
        lval_println(ret);
      }
      if (body->count) lval_del(ret);
    }
    if (!ret) return make_lval_sexpr();
    return ret;
  }

  return make_lval_copy(lval_fun);
}

Lval* eval_fun(Lenv* env, Lval* sexpr) {
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
    if (lval->count == 0) return lval;

    if (lval->node[0]->type == LVAL_SYM) {
      int special_sym = lookup_special_sym(lval->node[0]->sym);
      if (special_sym != -1) {
        /* printf("special sym: %s\n", lval->node[0]->sym); */
        lval_del(lval_pop(lval, 0));
        return eval_special(env, special_sym, lval);
      }
    }

    return eval_fun(env, lval);
  }
  return lval;
}
