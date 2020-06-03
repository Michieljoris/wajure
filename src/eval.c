#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "debug.h"
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

Lval* bind_lambda_formals(Lval* lval_fun, Lval* sexpr_args) {
  /* bind any args */
  int given = sexpr_args->count;
  int total = lval_fun->formals->count;

  while (sexpr_args->count) {
    /* Check if any formals (params) are left to bind */
    if (lval_fun->formals->count == 0) {
      lval_del(lval_fun);
      lval_del(sexpr_args);
      return make_lval_err(
          "Function passed too many args. Got %i, expected %i.", given, total);
    }

    /* Pop a parameter symbol */
    Lval* sym = lval_pop(lval_fun->formals, 0);

    /* Process symbol if it's a & and break out of while loop */
    if (strcmp(sym->sym, "&") == 0) {
      /* if the param is & there should be exactly one formal left  */
      if (lval_fun->formals->count != 1) {
        lval_del(lval_fun);
        lval_del(sexpr_args);
        return make_lval_err(
            "Function format invalid. "
            "Symbol '&' not followed by single symbol.");
      }
      /* Bind last param with rest of args */
      Lval* rest_sym = lval_pop(lval_fun->formals, 0);
      lenv_put(lval_fun->bindings, rest_sym, list_fn(NULL, sexpr_args));
      lval_del(sym);
      lval_del(rest_sym);
      /* Break out of while loop because all args are bound now */
      break;
    }
    /* Bind sym with arg */
    Lval* val = lval_pop(sexpr_args, 0);
    lenv_put(lval_fun->bindings, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  lval_del(sexpr_args);

  /* If there's still formals unbound and next one is & */
  if (lval_fun->formals->count > 0 &&
      strcmp(lval_fun->formals->node[0]->sym, "&") == 0) {
    /* The & needs to be followed by exactly one symbol */
    if (lval_fun->formals->count != 2) {
      Lval* lval_err = make_lval_err(
          "Function format invalid. "
          "Symbol '&' not followed by single symbol.");
      lval_del(lval_fun);
      return lval_err;
    }

    lval_del(lval_pop(lval_fun->formals, 0)); /* pop and free & */
    /* and bind last symbol to empty list */
    Lval* sym = lval_pop(lval_fun->formals, 0);
    Lval* val = make_lval_sexpr();
    lenv_put(lval_fun->bindings, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  return lval_fun;
}

Lval* eval_list(Lenv* bindings, Lval* list, bool with_tco) {
  Lval* ret = NULL;

  /* Eval all exprs of body but the last one (if with_tco is true)*/
  while (list->count > 1) {
    ret = lval_eval(bindings, lval_pop(list, 0));
    if (ret->type == LVAL_ERR) break;

    /* if expr is not last one of body discard the result */
    if (list->count) lval_del(ret);
  }

  /* Eval (or not) the last node in list */
  if (list->count && (ret == NULL || ret->type != LVAL_ERR)) {
    if (with_tco) {
      ret = lval_pop(list, 0);
      /* TODO make copy??? */
      ret->tco_env = lenv_copy(bindings);
      /* ret->tco_env = bindings; */

      /* printf("CREATED tco_env: %p\n", ret->tco_env); */
      /* lenv_print(ret->tco_env); */
      /* printf("----------\n"); */
    } else {
      Lval* expr = lval_pop(list, 0);
      ret = lval_eval(bindings, expr);
    }
  }

  return ret ? ret : make_lval_sexpr();
}

Lval* eval_lambda_call(Lval* lval_fun, Lval* sexpr_args, int with_tco) {
  /* Bind formals to args */
  lval_fun = bind_lambda_formals(lval_fun, sexpr_args);
  if (lval_fun->type == LVAL_ERR) return lval_fun;

  /* Eval body expressions, but only if all params are bound */
  if (lval_fun->formals->count == 0) {
    Lval* evalled_body =
        eval_list(lval_fun->bindings, lval_fun->body, with_tco);

    lval_del(lval_fun);

    return evalled_body;
  } else {
    return lval_fun;
  }
}

Lval* eval_macro_call(Lenv* env, Lval* lval_fun, Lval* sexpr_args,
                      int with_tco) {
  Lval* expanded_macro = eval_lambda_call(lval_fun, sexpr_args, WITHOUT_TCO);
  expanded_macro->tco_env = env;
  /* printf("RETURNING MACRO CALL WITH tco_env: %p\n", expanded_macro->tco_env);
   */
  /* lenv_print(expanded_macro->tco_env); */
  /* printf("----------\n"); */
  return expanded_macro;
}

Lval* eval_sym(Lenv* env, Lval* lval_sym) {
  Lval* lval_resolved_sym = lenv_get(env, lval_sym);
  lval_del(lval_sym);
  return lval_resolved_sym;
}

Lval* eval_vector(Lenv* env, Lval* lval_vector) {
  return eval_nodes(env, lval_vector, lval_vector->count);
}

Lval* eval_sexpr(Lenv* env, Lval* list) {
  Lval* lval_err;
  if (list->count == 0) return list;
  list = eval_nodes(env, list, 1);

  if (list->type == LVAL_ERR) return list;

  Lval* lval_fun = lval_pop(list, 0);
  if (lval_fun->type != LVAL_FUN) {
    lval_err = make_lval_err(
        "sexpr starts with incorrect type. "
        "Got %s, expected %s.",
        lval_type_to_name2(lval_fun), lval_type_to_name(LVAL_FUN));
    lval_del(list);
    lval_del(lval_fun);
    return lval_err;
  }
  switch (lval_fun->subtype) {
    case SYS:
    case LAMBDA:
      list = eval_nodes(env, list, list->count);
      if (list->type == LVAL_ERR) {
        lval_del(lval_fun);
        return list;
      }
    case MACRO:
    case SPECIAL:
    default:;
  }

  switch (lval_fun->subtype) {
    case SYS:
    case SPECIAL:
      return lval_fun->fun(env, list);
    case MACRO:
      return eval_macro_call(env, lval_fun, list, WITH_TCO);
    case LAMBDA:
      return eval_lambda_call(lval_fun, list, WITH_TCO);
    default:
      lval_err = make_lval_err("Unknown fun subtype %d for %s",
                               lval_fun->subtype, lval_fun->func_name);
      lval_del(list);
      lval_del(lval_fun);
      return lval_err;
  }
}

int id;
Lval* lval_eval(Lenv* env, Lval* lval) {
  /* id++; */
  /* printf("%d ", id); */
  /* lenv_print(env); */
  /* __printf(">>>>>> "); */
  Lval* ret = NULL;
  Lenv* tco_env = NULL;
  while (true) { /* TCO */
    /* printf(">"); */
    /* lval_println(lval); */
    if (tco_env) env = tco_env;
    switch (lval->type) {
      case LVAL_SYM:
        ret = eval_sym(env, lval);
        break;
      case LVAL_SEQ:
        switch (lval->subtype) {
          case LIST:
            lval = eval_sexpr(env, lval);
            if (lval->tco_env == NULL) {
              ret = lval;
            } else {
              /* printf("LOOPING!!!!!! %d\n", id); */
              /* printf("current tco_env: %p\n", tco_env); */
              /* if (tco_env) lenv_print(tco_env); */
              tco_env = lval->tco_env;
              lval->tco_env = NULL;
              /* printf("with lval= "); */
              /* lval_println(lval); */
              /* printf("and tco_env = %p\n", tco_env); */
              /* lenv_print(tco_env); */

              /* printf("and parent env = %p\n", tco_env->parent_env); */
              /* lenv_print(tco_env->parent_env); */

              /* printf("------\n"); */
              continue;
            }
            break;
          case VECTOR:
            ret = eval_vector(env, lval);
            break;
          case MAP:
            /* TODO: */
            ret = lval;
            break;
          case PLIST:
            ret = lval;
            break;
          default:
            lval_del(lval);
            ret = make_lval_err("Unknown seq subtype");
        }
        break;
      default:
        ret = lval;
    }
    if (tco_env) {
      /* printf("tco_env is set!!!!\n"); */
      /* printf("DELETING tc_env %p\n", tco_env); */
      /* lenv_print(tco_env); */
      /* printf("------------\n"); */
      /* lenv_del(tco_env); */
    }
    id--;
    /* printf("%d ", id); */
    return ret;
  }
}
/* You can delete existing tco_env if next one is not pointing to it, like in
 * recursion. */
/*  When in nested closures, delete whole chain when getting getting out of tco
 * loop.  */
