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
      lenv_put(lval_fun->env, rest_sym, list_fn(NULL, sexpr_args));
      lval_del(sym);
      lval_del(rest_sym);
      /* Break out of while loop because all args are bound now */
      break;
    }
    /* Bind sym with arg */
    Lval* val = lval_pop(sexpr_args, 0);
    lenv_put(lval_fun->env, sym, val);
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
    lenv_put(lval_fun->env, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  return lval_fun;
}

Lval* eval_body(Lenv* bindings, Lval* body, bool with_tco) {
  Lval* ret = NULL;

  /* printf("body\n"); */
  /* lval_println(body); */
  /* Eval all exprs of body but the last one*/
  while (body->count > 1) {
    ret = lval_eval(bindings, lval_pop(body, 0));
    if (ret->type == LVAL_ERR) break;

    /* if expr is not last one of body discard the result */
    if (body->count) lval_del(ret);
  }
  if (body->count && (ret == NULL || ret->type != LVAL_ERR)) {
    if (with_tco) {
      /* Don't eval the last one, but let the while true loop in eval take care
       * of it */
      ret = lval_pop(body, 0);
      ret->tco_env = bindings;
    } else {
      Lval* expr = lval_pop(body, 0);
      ret = lval_eval(bindings, expr);
    }
  }

  lval_del(body);
  return ret ? ret : make_lval_sexpr();
}

Lval* eval_lambda_call(Lval* lval_fun, Lval* sexpr_args) {
  /* Bind formals to args */
  lval_fun = bind_lambda_formals(lval_fun, sexpr_args);
  if (lval_fun->type == LVAL_ERR) return lval_fun;

  /* Eval body expressions, but only if all params are bound */
  if (lval_fun->formals->count == 0) {
    Lval* body = lval_fun->body;
    Lval* evalled_body = eval_body(lval_fun->env, body, WITH_TCO);

    /* lenv_del(lval_fun->env); */
    free(lval_fun->formals);
    free(lval_fun);

    /* if (bound_macro->type == LVAL_ERR) return bound_macro; */

    return evalled_body;
  } else {
    return lval_fun;
  }
}

Lval* eval_macro_call(Lval* lval_fun, Lval* sexpr_args, int with_tco) {
  /* Bind formals to args */
  lval_fun = bind_lambda_formals(lval_fun, sexpr_args);
  if (lval_fun->type == LVAL_ERR) return lval_fun;

  /* Eval body expressions, but only if all params are bound */
  if (lval_fun->formals->count == 0) {
    Lval* body = lval_fun->body;
    Lval* bound_macro = eval_body(lval_fun->env, body, WITHOUT_TCO);

    /* lenv_del(lval_fun->env); */
    lval_del(lval_fun->formals);
    free(lval_fun);

    if (bound_macro->type == LVAL_ERR) return bound_macro;

    if (with_tco) bound_macro->tco_env = lval_fun->env;
    return bound_macro;
  } else {
    return lval_fun;
  }
}

Lval* eval_sym(Lenv* env, Lval* lval_sym) {
  Lval* lval_resolved_sym = lenv_get(env, lval_sym);
  lval_del(lval_sym);
  return lval_resolved_sym;
}

Lval* eval_vector(Lenv* env, Lval* lval_vector) {
  return eval_nodes(env, lval_vector, lval_vector->count);
}

Lval* eval_list(Lenv* env, Lval* list) {
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
      return eval_macro_call(lval_fun, list, WITH_TCO);
    case LAMBDA:
      return eval_lambda_call(lval_fun, list);
    default:
      lval_err = make_lval_err("Unknown fun subtype %d for %s",
                               lval_fun->subtype, lval_fun->func_name);
      lval_del(list);
      lval_del(lval_fun);
      return lval_err;
  }
}

Lval* lval_eval(Lenv* env, Lval* lval) {
  /* printf("evalling! %d\n", i++); */
  /* lval_println(lval); */
  while (true) { /* TCO */
    /* printf("eval!!!!\n"); */
    switch (lval->type) {
      case LVAL_SYM:
        return eval_sym(env, lval);
      case LVAL_SEQ:
        switch (lval->subtype) {
          case LIST:
            lval = eval_list(env, lval);
            if (lval->tco_env == NULL) return lval;
            printf("TCO\n");
            env = lval->tco_env;
            lval->tco_env = NULL;
            break;
          case VECTOR:
            return eval_vector(env, lval);
          case MAP:
            /* TODO: */
            return lval;
          default:
            lval_del(lval);
            return make_lval_err("Unknown seq subtype");
        }
        break;
      default:
        return lval;
    }
  }
  return lval;
}
