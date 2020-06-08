#include "eval.h"

#include "assert.h"
#include "debug.h"
#include "env.h"
#include "io.h"
#include "iter.h"
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
  Cell* i = iter_new(lval_list);
  Lval* lval = iter_next(i);
  Cell** lp = &(new_list->head);
  while (lval) {
    Lval* x = lval_eval(env, lval);
    if (x->type == LVAL_ERR) {
      lval_del(lval_list);
      lval_del(new_list);
      iter_end(i);
      return x;
    }
    Cell* next_cell = make_cell();
    *lp = next_cell;
    lp = &(next_cell->cdr);
    next_cell->car = x;
    lval = iter_next(i);
  }
  iter_end(i);
  lval_del(lval_list);

  return new_list;
}

Lval* bind_lambda_params(Lval* lval_fun, Lval* arg_list) {
  /* Pop an arg */
  Cell* a = iter_new(arg_list);
  Lval* arg = iter_next(a);

  /* Pop a parameter symbol */
  Cell* p = iter_new(lval_fun->params);
  Lval* param = NIL;

  Lenv* bindings = lval_fun->bindings;

  // Try to bind all the arg in arg_list
  while (arg) {
    param = iter_next(p);
    /* Return error if we've got an arg but not a param */
    if (!param) {
      return make_lval_err(
          "Function passed too many args. Got %i, expected %i.",
          list_count(arg_list->head), list_count(lval_fun->params->head));
    }

    /* Process symbol if it's a & and break out of while loop */
    if (_strcmp(param->sym, "&") == 0) {
      param = iter_next(p);
      /* if the param is & there should be exactly one formal left  */
      if (!param || iter_peek(p)) {
        return make_lval_err(
            "Function format invalid. "
            "Symbol '&' not followed by single symbol.");
      }
      Lval* rest_args = make_lval_list();
      rest_args->head = iter_current_cell(a);
      /* Bind last param with rest of args */
      bindings = lenv_assoc(bindings, param, rest_args);
      /* Break out of while loop because all args are bound now */
      break;
    }
    /* Bind sym with arg */
    bindings = lenv_assoc(bindings, param, arg);
    arg = iter_next(a);
  }
  iter_end(a);

  param = iter_next(p);
  /* If there's still params unbound and next one is & */
  if (param && _strcmp(param->sym, "&") == 0) {

    param = iter_next(p);  // rest param
    /* The & needs to be followed by exactly one symbol */
    if (!param) {
      Lval* lval_err = make_lval_err(
          "Function format invalid. "
          "Symbol '&' not followed by single symbol.");
      iter_end(p);
      lfree(LENV, bindings);
      return lval_err;
    }

    /* and bind last symbol to empty list */
    Lval* empty_lval_list = make_lval_list();
    bindings = lenv_assoc(bindings, param, empty_lval_list);
    iter_next(p);
  }

  Lval* params = make_lval_vector();
  params->head = iter_current_cell(p);
  iter_end(p);
  return make_lval_lambda(bindings, params, lval_fun->body, LAMBDA);
}

Lval* eval_body(Lenv* env, Lval* list, int with_tco) {
  Cell* i = iter_new(list);
  Lval* lval = iter_next(i);
  Lval* ret = NULL;
  /* Eval all exprs of body but the last one (if with_tco is true)*/
  while (lval) {
    if (iter_peek(i)) {
      /* if expr is not last one of body discard the result */
      lval_del(ret);
    } else {
      if (with_tco) {
        ret = lval;
        ret->tco_env = env;
        break;
       }
    }
    ret = lval_eval(env, lval);
    if (ret->type == LVAL_ERR) break;

    lval = iter_next(i);
  }
  iter_end(i);

  return ret ? ret : make_lval_list();
}

Lval* eval_macro_call(Lenv* env, Lval* lval_fun, Lval* arg_list) {
  lval_fun = bind_lambda_params(lval_fun, arg_list);

  if (lval_fun->params->head) {
    return make_lval_err("Macro needs all its params bound");
  }
  if (lval_fun->type == LVAL_ERR) return lval_fun;
  Lval* expanded_macro =
      eval_body(lval_fun->bindings, lval_fun->body, WITHOUT_TCO);

  // Expanded macro closes over the environment where it is executed
  expanded_macro->tco_env = env;
  return expanded_macro;
}

Lval* eval_lambda_call(Lval* lval_fun, Lval* arg_list, int with_tco) {
  lval_fun = bind_lambda_params(lval_fun, arg_list);
  if (lval_fun->type == LVAL_ERR) return lval_fun;

  /* Eval body expressions, but only if all params are bound */
  if (!lval_fun->params->head) {
    Lval* evalled_body =
        eval_body(lval_fun->bindings, lval_fun->body, with_tco);

    return evalled_body;
  } else {
    return lval_fun;
  }
}

Lval* eval_macro_call2(Lenv* env, Lval* lval_fun, Lval* arg_list,
                       int with_tco) {
  /* printf(">>>>>>>>>>>>>>>>>evalling macro call\n"); */
  /* lval_println(lval_fun); */
  /* lval_println(arg_list); */
  /* printf("Expanding..\n"); */
  Lval* expanded_macro = eval_lambda_call(lval_fun, arg_list, WITHOUT_TCO);
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
  lval_vector = eval_nodes(env, lval_vector);
  lval_vector->subtype = VECTOR;
  return lval_vector;
}

// We've got a list. We expect first node to be a fn call, and the rest args to
// the fn.
Lval* eval_fn_call(Lenv* env, Lval* lval_list) {
  /* printf("evalling fn call: "); */
  /* lval_println(lval_list); */
  Lval* lval_err;
  if (lval_list->head == NIL) {
    // TODO: release list here
    return make_lval_list();  // empty;
  }

  Lval* lval_fun =
      lval_eval(env, list_first(lval_list->head));  // eval first node
  if (lval_fun->type == LVAL_ERR) return lval_fun;

  if (lval_fun->type != LVAL_FUNCTION) {
    lval_err = make_lval_err(
        "sexpr starts with incorrect type. "
        "Got %s, expected %s.",
        lval_type_to_name(lval_fun), lval_type_constant_to_name(LVAL_FUNCTION));
    lval_del(lval_list);
    lval_del(lval_fun);
    return lval_err;
  }

  Lval* arg_list = make_lval_list();
  arg_list->head = list_rest(lval_list->head);
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
  /* lval_println(arg_list); */
  switch (lval_fun->subtype) {
    case SYS:
    case SPECIAL: {
      Lval* ret = lval_fun->fun(env, arg_list);
      return ret;
    }
    case MACRO:
      return eval_macro_call(env, lval_fun, arg_list);
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
