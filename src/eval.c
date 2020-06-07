#include "eval.h"

#include "assert.h"
#include "debug.h"
#include "env.h"
#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "list_fns.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"

Lval* lval_eval(Lenv* e, Lval* v);

Lval* eval_nodes(Lenv* env, Lval* lval_list) {
  Lval* new_list = make_lval_list();
  if (lval_list->list) {
    Cell* from_cell = lval_list->list;
    Cell* last_cell;
    Cell* to_cell = NIL;
    Cell* dummy = last_cell = make_cell();
    do {
      Lval* lval = lval_eval(env, (Lval*)from_cell->car);
      if (lval->type == LVAL_ERR) {
        lval_del(lval_list);
        lval_del(new_list);
        return lval;
      }
      to_cell = make_cell();
      last_cell->cdr = to_cell;
      last_cell = to_cell;
      to_cell->car = lval;
      from_cell = from_cell->cdr;
    } while (from_cell);
    lval_del(lval_list);
    new_list->list = dummy->cdr;
    lfree(CELL, dummy);
  }
  return new_list;
}

Lval* bind_lambda_params(Lval* lval_fun, Lval* arg_list) {
  /* bind any args */
  int given = arg_list->count;
  int total = lval_fun->params->count;

  while (arg_list->count) {
    /* Check if any formals (params) are left to bind */
    if (lval_fun->params->count == 0) {
      lval_del(lval_fun);
      lval_del(arg_list);
      return make_lval_err(
          "Function passed too many args. Got %i, expected %i.", given, total);
    }

    /* Pop a parameter symbol */
    Lval* sym = lval_pop(lval_fun->params, 0);

    /* Process symbol if it's a & and break out of while loop */
    if (_strcmp(sym->sym, "&") == 0) {
      /* if the param is & there should be exactly one formal left  */
      if (lval_fun->params->count != 1) {
        lval_del(lval_fun);
        lval_del(arg_list);
        return make_lval_err(
            "Function format invalid. "
            "Symbol '&' not followed by single symbol.");
      }
      /* Bind last param with rest of args */
      Lval* rest_sym = lval_pop(lval_fun->params, 0);
      lenv_put(lval_fun->bindings, rest_sym, list_fn(NULL, arg_list));
      lval_del(sym);
      lval_del(rest_sym);
      /* Break out of while loop because all args are bound now */
      break;
    }
    /* Bind sym with arg */
    Lval* val = lval_pop(arg_list, 0);
    lenv_put(lval_fun->bindings, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  lval_del(arg_list);

  /* If there's still formals unbound and next one is & */
  if (lval_fun->params->count > 0 &&
      _strcmp(lval_fun->params->node[0]->sym, "&") == 0) {
    /* The & needs to be followed by exactly one symbol */
    if (lval_fun->params->count != 2) {
      Lval* lval_err = make_lval_err(
          "Function format invalid. "
          "Symbol '&' not followed by single symbol.");
      lval_del(lval_fun);
      return lval_err;
    }

    lval_del(lval_pop(lval_fun->params, 0)); /* pop and free & */
    /* and bind last symbol to empty list */
    Lval* sym = lval_pop(lval_fun->params, 0);
    Lval* val = make_lval_list();
    lenv_put(lval_fun->bindings, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  return lval_fun;
}

Lval* eval_body(Lenv* bindings, Lval* list, int with_tco) {
  /* printf("eval body\n"); */
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
      /* printf("RET ="); */
      /* lval_println(ret); */
    }
  }

  return ret ? ret : make_lval_list();
}

Lval* eval_lambda_call(Lval* lval_fun, Lval* sexpr_args, int with_tco) {
  /* Bind formals to args */
  /* printf("eval lambda call: "); */
  /* lval_println(lval_fun); */
  lval_fun = bind_lambda_params(lval_fun, sexpr_args);
  /* printf("evalled formals\n"); */
  if (lval_fun->type == LVAL_ERR) return lval_fun;

  /* Eval body expressions, but only if all params are bound */
  if (lval_fun->params->count == 0) {
    /* printf("bindings:  "); */
    /* lenv_print(lval_fun->bindings); */
    /* printf("--------------evalling body:"); */
    /* lval_println(lval_fun->body); */
    Lval* evalled_body =
        eval_body(lval_fun->bindings, lval_fun->body, with_tco);

    /* printf("--------------evalled body: :"); */
    /* lval_println(evalled_body); */
    /* printf("evalled body tco env: :"); */
    /* lenv_print(evalled_body->tco_env); */
    return evalled_body;
  } else {
    return lval_fun;
  }
}

Lval* eval_macro_call(Lenv* env, Lval* lval_fun, Lval* sexpr_args,
                      int with_tco) {
  /* printf(">>>>>>>>>>>>>>>>>evalling macro call\n"); */
  /* lval_println(lval_fun); */
  /* lval_println(sexpr_args); */
  /* printf("Expanding..\n"); */
  Lval* expanded_macro = eval_lambda_call(lval_fun, sexpr_args, WITHOUT_TCO);
  /* printf("Original lval_fun: "); */
  /* lval_println(lval_fun); */
  /* printf  ("Expanded macro:\n"); */
  /* lval_println(expanded_macro); */

  // After the macro is expanded they close over the environment where they are
  // executed
  expanded_macro->tco_env = env;
  /* printf("RETURNING MACRO CALL WITH tco_env: %p\n", expanded_macro->tco_env);
   */
  /* lenv_print(expanded_macro->tco_env); */
  /* printf("<<<<<<<<<<<<<<<<<<<<<<<<<\n"); */
  return expanded_macro;
}

Lval* eval_symbol(Lenv* env, Lval* lval_symbol) {
  /* printf("eval_symbol: symbol :"); */
  /* lval_println(lval_symbol); */
  /* lenv_print(env); */
  /* lenv_print(env->parent_env); */
  Lval* lval_resolved_sym = lenv_get(env, lval_symbol);
  lval_del(lval_symbol);
  return lval_resolved_sym;
}

Lval* eval_vector(Lenv* env, Lval* lval_vector) {
  return eval_nodes(env, lval_vector);
}

// We've got a list. We expect first node to be a fn call, and the rest args to
// the fn.
Lval* eval_fn_call(Lenv* env, Lval* lval_list) {
  /* printf("evalling fn call: "); */
  /* lval_println(lval_list); */
  Lval* lval_err;
  if (lval_list->list == NIL) {
    // TODO: release list here
    return make_lval_list();  // empty;
  }

  Lval* lval_fun =
      lval_eval(env, list_first(lval_list->list));  // eval first node
  if (lval_fun->type == LVAL_ERR) return lval_list;

  if (lval_fun->type != LVAL_FUNCTION) {
    lval_err = make_lval_err(
        "sexpr starts with incorrect type. "
        "Got %s, expected %s.",
        lval_type_to_name2(lval_fun), lval_type_to_name(LVAL_FUNCTION));
    lval_del(lval_list);
    lval_del(lval_fun);
    return lval_err;
  }

  Lval* arg_list = make_lval_list();
  arg_list->list = list_rest(lval_list->list);
  switch (lval_fun->subtype) {
    case SYS:
    case LAMBDA:
      arg_list = eval_nodes(env, arg_list);
      if (arg_list->type == LVAL_ERR) {
        lval_del(lval_fun);
        return arg_list;
      }
    case MACRO:
    case SPECIAL:
    default:;
  }
  /* printf("evalled the arguments of fn\n"); */
  switch (lval_fun->subtype) {
    case SYS:
    case SPECIAL: {
      Lval* ret = lval_fun->fun(env, arg_list);
      return ret;
    }
    case MACRO:
      return eval_macro_call(env, lval_fun, arg_list, WITH_TCO);
    case LAMBDA:
      return eval_lambda_call(lval_fun, arg_list, WITH_TCO);
    default:
      lval_err = make_lval_err("Unknown fun subtype %d for %s",
                               lval_fun->subtype, lval_fun->func_name);
      lval_del(arg_list);
      lval_del(lval_fun);
      return lval_err;
  }
}

Lval* lval_eval(Lenv* env, Lval* lval) {
  Lenv* tco_env = NULL;
  while (1) { /* TCO */
    if (tco_env) env = tco_env;
    switch (lval->type) {
      case LVAL_SYMBOL:
        return eval_symbol(env, lval);
        break;
      case LVAL_COLLECTION:
        switch (lval->subtype) {
          case LIST:
            lval = eval_fn_call(env, lval);
            if (lval->tco_env != NULL) {
              tco_env = lval->tco_env;
              continue;
            }
            return lval;
            break;
          case VECTOR:
            return eval_vector(env, lval);
            break;
          case MAP:
            /* TODO: */
            return lval;
            break;
          default:
            lval_del(lval);
            return make_lval_err("Unknown seq subtype");
        }
        break;
      default:
        return lval;
    }
  }
}
/* You can delete existing tco_env if next one is not pointing to it, like in
 * recursion. */
/*  When in nested closures, delete whole chain when getting getting out of tco
 * loop.  */
