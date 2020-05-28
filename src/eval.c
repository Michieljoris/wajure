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

Lval* lval_eval(Lenv* e, Lval* v);

Lval* eval_nodes(Lenv* env, Lval* lval, int count) {
  /* eval nodes */
  for (int i = 0; i < count; i++) {
    lval->node[i] = lval_eval(env, lval->node[i]);
  }
  /* error checking */
  for (int i = 0; i < count; i++) {
    if (lval->node[i]->type == LVAL_ERR) {
      return lval_take(lval, i);
    }
  }
  return lval;
}

/* TODO: there's a memory leak with lval_fun!!!! */

Lval* eval_sys_call(Lenv* env, Lval* lval_fun, Lval* sexpr) {
  Lval* evalled_nodes = eval_nodes(env, sexpr, sexpr->count);
  if (evalled_nodes->type == LVAL_ERR) return evalled_nodes;
  return lval_fun->fun(env, evalled_nodes);
}

Lval* eval_special_call(Lenv* env, Lval* lval_fun, Lval* sexpr_args) {
  return lval_fun->fun(env, sexpr_args);
}

Lval* eval_lambda_call(Lenv* env, Lval* lval_fun, Lval* sexpr_args) {
  /* eval lambda call */
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

Lval* eval_macro_call(Lenv* env, Lval* lval_fun, Lval* sexpr_args) {
  /* printf(">>>fun and args of macro call:\n"); */
  /* lval_println(lval_fun); */
  /* lval_println(sexpr_args); */
  /* printf(">>>lambda call on macro :\n"); */
  Lval* bound_macro = eval_lambda_call(env, lval_fun, sexpr_args);
  if (bound_macro->type == LVAL_ERR) return bound_macro;
  printf(">>>result of lambda call on macro :\n");
  lval_println(bound_macro);

  /* lval_del(sexpr_args); */
  Lval* ret = lval_eval(env, bound_macro);
  if (!ret) return make_lval_sexpr();
  return ret;
}

Lval* eval_sym(Lenv* env, Lval* lval_sym) {
  Lval* lval_resolved_sym = lenv_get(env, lval_sym);
  lval_del(lval_sym);
  return lval_resolved_sym;
}

Lval* eval_vector(Lenv* env, Lval* lval_vector) {
  return eval_nodes(env, lval_vector, lval_vector->count);
}

Lval* eval_sexpr(Lenv* env, Lval* sexpr) {
  if (sexpr->count == 0) return sexpr;
  sexpr = eval_nodes(env, sexpr, 1);

  if (sexpr->type == LVAL_ERR) return sexpr;

  Lval* lval_fun = lval_pop(sexpr, 0);
  if (lval_fun->type != LVAL_FUN)
    return make_lval_err(
        "sexpr starts with incorrect type. "
        "Got %s, expected %s.",
        lval_type_to_name2(sexpr), lval_type_to_name(LVAL_FUN));

  switch (lval_fun->subtype) {
    case SYS:
      return eval_sys_call(env, lval_fun, sexpr);
    case MACRO:
      return eval_macro_call(env, lval_fun, sexpr);
    case SPECIAL:
      return eval_special_call(env, lval_fun, sexpr);
    case LAMBDA:
      return eval_lambda_call(env, lval_fun, sexpr);
    default:
      return make_lval_err("Unknown fun subtype %d for %s", lval_fun->subtype,
                           lval_fun->func_name);
  }
}

Lval* lval_eval(Lenv* env, Lval* lval) {
  /* printf("evalling!\n"); */
  /* lval_println(lval); */
  switch (lval->type) {
    case LVAL_SYM:
      return eval_sym(env, lval);
    case LVAL_SEQ:
      switch (lval->subtype) {
        case VECTOR:
          return eval_vector(env, lval);
        case LIST:
          return eval_sexpr(env, lval);
        case MAP:
          /* TODO: */
          return lval;
        default:
          return make_lval_err("Unknown seq subtype");
      }
      break;
    default:
      return lval;
  }
}

/* int special_sym = lookup_special_sym(lval->node[0]->sym); */
/* if (special_sym != -1) { */
/*   lval_del(lval_pop(lval, 0)); */
/*   return eval_special(env, special_sym, lval); */
/* } */

/* Lval* eval_fun(Lenv* env, Lval* sexpr) { */
/*   sexpr = eval_nodes(env, sexpr, sexpr->count); */
/*   /\* if error return error!!! *\/ */

/*   /\* first expr should have evalled to a fun*\/ */
/*   Lval* lval_fun = lval_pop(sexpr, 0); */
/*   if (lval_fun->type != LVAL_FUN && lval_fun->type != LVAL_MACRO) { */
/*     lval_del(lval_fun); */
/*     lval_del(sexpr); */
/*     return make_lval_err( */
/*         "sexpr starts with incorrect type. " */
/*         "Got %s, expected %s.", */
/*         lval_type_to_name(lval_fun->type), lval_type_to_name(LVAL_FUN)); */
/*   } */

/*   /\* sexpr has all elements now except for first (a LVAL_FUN) *\/ */
/*   Lval* lval = lval_call(env, lval_fun, sexpr); */
/*   lval_del(lval_fun); */
/*   return lval; */
/* } */
