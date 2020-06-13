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
  printf("eval nodes, new list:");
  Lval* new_list = make_lval_list();
  scoped_iter Cell* i = iter_new(lval_list);
  Lval* lval = iter_next(i);
  Cell** lp = &(new_list->head);
  while (lval) {
    Lval* x = lval_eval(env, lval);
    if (x->type == LVAL_ERR) {
      release(new_list);
      return x;
    }
    Cell* next_cell = make_cell();
    *lp = next_cell;
    lp = &(next_cell->cdr);
    next_cell->car = x;
    lval = iter_next(i);
  }

  return new_list;
}

Lval* bind_lambda_params(Lval* lval_fun, Lval* arg_list) {
  printf("\n---------------------bind_lambda_params\n");
  /* Pop an arg */
  scoped_iter Cell* a = iter_new(arg_list);
  Lval* arg = iter_next(a);

  /* Pop a parameter symbol */
  scoped_iter Cell* p = iter_new(lval_fun->params);
  Lval* param = NIL;

  Lenv* bindings = lval_fun->bindings;
  printf("\nBINDINGs:") lenv_print(bindings);
  printf("\n");

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
      lenv_put(bindings, param, rest_args);
      /* Break out of while loop because all args are bound now */
      break;
    }
    /* Bind sym with arg */
    lenv_put(bindings, param, arg);
    arg = iter_next(a);
  }
  printf("\nBINDINGs:") lenv_print(bindings);
  printf("\n");

  param = iter_next(p);
  /* If there's still params unbound and next one is & */
  if (param && _strcmp(param->sym, "&") == 0) {
    param = iter_next(p);  // rest param
    /* The & needs to be followed by exactly one symbol */
    if (!param) {
      Lval* lval_err = make_lval_err(
          "Function format invalid. "
          "Symbol '&' not followed by single symbol.");
      /* iter_end(p); */
      /* lfree(LENV, bindings); */
      return lval_err;
    }

    /* and bind last symbol to empty list */
    Lval* empty_lval_list = make_lval_list();
    lenv_put(bindings, param, empty_lval_list);
    iter_next(p);
  }

  Lval* params = make_lval_vector();
  params->head = retain(iter_current_cell(p));
  printf("\nPARAMS:");
  lval_println(params);
  return make_lval_lambda(bindings, params, lval_fun->body, LAMBDA);
}

Lval* eval_body(Lenv* env, Lval* list, int with_tco) {
  scoped_iter Cell* i = iter_new(list);
  Lval* lval = iter_next(i);
  Lval* ret = NULL;
  /* Eval all exprs of body but the last one (if with_tco is true)*/
  while (lval) {
    if (iter_peek(i)) {
      release(lval);
      /* if expr is not last one of body discard the result */
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
  printf("\n&&&&&&&&&&&&&&&&&&&&&&&&&eval_lambda_call\n");
  lval_fun = bind_lambda_params(lval_fun, arg_list);
  printf("\n&&&&&&&&&&&&&&&&&&&&&&&&&eval_lambda_call\n");
  lval_println(lval_fun);
  lenv_print(lval_fun->bindings);
  if (lval_fun->type == LVAL_ERR) return lval_fun;
  /* Eval body expressions, but only if all params are bound */
  if (!lval_fun->params->head) {
    Lval* evalled_body =
        eval_body(lval_fun->bindings, lval_fun->body, with_tco);
    printf("\n&&&&&&&&&&&&&&&&&&&&&&&&&eval_lambda_call\n");
    lval_println(evalled_body);
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
  Lval* lval_resolved_sym = lenv_get(env, lval_symbol);
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
  printf("evalling fn call: ");
  lval_println(lval_list);
  Lval* lval_err;
  if (lval_list->head == NIL) {
    return make_lval_list();  // empty;
  }
  Lval* lval_symbol = list_first(lval_list->head);
  scoped Lval* lval_fun = lval_eval(env, lval_symbol);  // eval first node
  release(lval_symbol);

  if (lval_fun->type == LVAL_ERR) {
    return lval_fun;
  }

  if (lval_fun->type != LVAL_FUNCTION) {
    lval_err = make_lval_err(
        "sexpr starts with incorrect type. "
        "Got %s, expected %s.",
        lval_type_to_name(lval_fun), lval_type_constant_to_name(LVAL_FUNCTION));
    return lval_err;
  }

  /* Lval* ret = NIL; */
  scoped Lval* arg_list = make_lval_list();
  arg_list->head = list_rest(lval_list->head);
  scoped Lval* evalled_arg_list = NIL;
  switch (lval_fun->subtype) {
    case SYS:
    case LAMBDA:
      evalled_arg_list = eval_nodes(env, arg_list);
      if (evalled_arg_list->type == LVAL_ERR) {
        retain(evalled_arg_list);
        return evalled_arg_list;
      }
    default:;
  }
  /* printf("evalled the arguments of fn\n"); */
  /* lval_println(arg_list); */
  switch (lval_fun->subtype) {
    case SYS:
      return lval_fun->fun(env, evalled_arg_list);
      break;
    case SPECIAL: {
      return lval_fun->fun(env, arg_list);
      break;
    }
    case MACRO:
      return eval_macro_call(env, lval_fun, arg_list);
      break;
    case LAMBDA:
      return eval_lambda_call(lval_fun, evalled_arg_list, WITH_TCO);
      break;
    default:
      return make_lval_err("Unknown fun subtype %d for %s", lval_fun->subtype,
                           lval_fun->func_name);
  }
}

Lval* lval_eval(Lenv* env, Lval* lval) {
  /* printf("\n->>>>>>>>EVAL: %d", i++); */
  printf("\n->>>>>>>>EVAL: ");
  lval_println(lval);
  Lval* ret = NIL;
  Lenv* tco_env = NULL;
  while (1) { /* TCO */
    if (tco_env) env = tco_env;
    switch (lval->type) {
      case LVAL_SYMBOL:
        ret = eval_symbol(env, lval);
        if (tco_env) {
          printf("SYMBOL: Releasing tco_env!!!! \n");
          release(tco_env);
          /* retain(ret); */
        }
        return ret;
      case LVAL_COLLECTION:
        switch (lval->subtype) {
          case LIST:
            ret = eval_fn_call(env, lval);
            printf("Evaled fn call, result:");
            lval_println(ret);
            if (tco_env) {
              printf("LIST: Releasing tco_env!!!! \n");
              lenv_print(tco_env);
              /* printf("PUUUUT lval_env rc: %d\n", get_ref_count(tco_env)); */
              release(tco_env);
            }
            lval = ret;
            if (lval->tco_env != NULL) {
              printf("\nRECEIVED TCO_ENV: ");
              lval_println(lval);
              lval_println(ret);
              lenv_print(lval->tco_env);
              tco_env = lval->tco_env;
              continue;
            }
            return lval;
          case VECTOR:
            ret = eval_vector(env, lval);
            if (tco_env) {
              /* printf("VECTOR: Releasing tco_env!!!! %d\n", i); */
              release(tco_env);
            }
            return ret;
          case MAP:
            /* TODO: */
            return lval;
          default:
            return make_lval_err("Unknown seq subtype");
        }
        break;
      default:
        retain(lval);
        if (tco_env) {
          /* printf("LITERAL: Releasing tco_env!!!! %d\n", i); */
          release(tco_env);
        }
        return lval;
    }
  }
}

/* printf("Releasing tco_env!!!!\n") */

/* printf("EVALLING SYMBOL with tco_env!!!!!!!\n"); */
/* lval_println(lval); */
/* lval_println(ret); */
/* printf("REFCOUNT of tco_env rc: %d\n", get_ref_count(tco_env)); */
/* release(lval); */
/* lenv_print(tco_env); */
/* printf("--\n"); */

/* Lenv* parent_env = tco_env->parent_env; */
/* while (parent_env) { */
/*   lenv_print(parent_env); */
/*   parent_env = parent_env->parent_env; */
/*   printf("--\n"); */
/* } */

/* printf("Releasing tco_env!!!!\n"); */
