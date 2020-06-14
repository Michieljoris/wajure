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
#include "read.h"

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

// Returns NIL if list is empty, otherwise evals and discards all expressions
// but the last one, which is returned.
Lval* eval_list_but_last(Lenv* env, Lval* list) {
  scoped_iter Cell* i = iter_new(list);
  Lval* lval = iter_next(i);

  while (lval) {
    if (iter_peek(i)) {
      lval = lval_eval(env, lval);
      if (lval->type == LVAL_ERR) return lval;
      release(lval);
    } else
      return lval;
    lval = iter_next(i);
  }
  return NIL;
}

Lval* eval_string(Lenv* env, char* str) {
  int pos = 0;
  scoped Lval* lval_list = lval_read_list(str, &pos, '\0');

  // Evaluate all expressions for side effects and definitions but the last.
  Lval* last_expr = eval_list_but_last(env, lval_list);
  if (!last_expr) return make_lval_list();  // empty list is our nil
  if (last_expr->type == LVAL_ERR) return last_expr;

  /* Evaluate the last expression and return it */
  Lval* ret = lval_eval(env, last_expr);
  printf("\nDone eval_string\n");
  return ret;
}

Lval* read_rest_args(Lval* param, Cell* p, Cell* a) {
  /* if the param is & there should be exactly one formal left  */
  if (!param || iter_peek(p)) {
    return NIL;
  }
  Lval* rest_args = make_lval_list();
  rest_args->head = retain(iter_current_cell(a));
  return rest_args;
}

/* Lval* make_bound_lval_lambda(Lval* params, Lval* body, Lval* arg_list) { */
/* while (arg) { */
/*   /\* Return error if we've got an arg but not a param *\/ */
/*   if (!param) { */
/*     return make_lval_err("Function passed too many args. Got %i, expected
 * %i.", */
/*                          list_count(arg_list->head),
 * list_count(params->head)); */
/*   } */

/*   if (_strcmp(param->sym, "&") == 0) { */
/*     param = iter_next(p); */
/*     arg = read_rest_args(param, p, a); */
/*     if (!arg) { */
/*       release(bindings_env); */
/*       return make_lval_err( */
/*           "Function format invalid. " */
/*           "Symbol '&' not followed by single symbol."); */
/*     } */
/*   } */
/*   bindings_env->kv = alist_prepend(bindings_env->kv, retain(param), arg); */
/*   arg = iter_next(a); */
/*   param = iter_next(p); */
/* } */

/* if (param && _strcmp(param->sym, "&") == 0) { */
/*   param = iter_next(p);  // rest param */
/*   /\* The & needs to be followed by exactly one symbol *\/ */
/*   if (!param) { */
/*     release(bindings_env); */
/*     return make_lval_err( */
/*         "Function format invalid. " */
/*         "Symbol '&' not followed by single symbol."); */
/*   } */
/*   /\* and bind last symbol to empty list *\/ */
/*   alist_prepend(bindings_env->kv, retain(param), make_lval_list()); */
/*   iter_next(p); */
/* } */

// Bind any remaining params to rest arg if there is one  ==========

/* params = make_lval_vector(); */
/* params->head = retain(iter_current_cell(p)); */
/* return make_lval_lambda(bindings_env, params, body, LAMBDA); */

/* Lval* eval_lambda_call(Lenv* env, Lval* lval_fun, Lval* arg_list) { */
/* printf("\n&&&&&&&&&&&&&&&&&&&&&&&&&eval_lambda_call\n"); */

/* Lval* bound_lval_lambda = make_bound_lval_lambda( */
/*     lval_fun->params, retain(lval_fun->body), arg_list); */

Lval* eval_lambda_call(Lval* lval_fun, Lval* arg_list) {
  printf("\n---------------------bind_lambda_params\n");
  scoped Lenv* bindings_env = lenv_new();

  scoped_iter Cell* p = iter_new(lval_fun->params);
  Lval* param = iter_next(p);

  scoped_iter Cell* a = iter_new(arg_list);
  Lval* arg = iter_next(a);

  // Try to bind all the arg in arg_list ====================
  while (param) {
    if (_strcmp(param->sym, "&") == 0) {
      param = iter_next(p);
      arg = read_rest_args(param, p, a);
      if (!arg)
        return make_lval_err(
            "Function format invalid. "
            "Symbol '&' not followed by single symbol.");
    }
    if (arg)
      bindings_env->kv =
          alist_prepend(bindings_env->kv, retain(param), retain(arg));
    else
      break;
    arg = iter_next(a);
    param = iter_next(p);
  }

  if (!param && arg)
    return make_lval_err("Function passed too many args. Got %i, expected %i.",
                         list_count(arg_list->head),
                         list_count(lval_fun->params->head));

  bindings_env->parent_env = retain(lval_fun->closure_env);

  /* Eval body expressions, but only if all params are bound */
  if (!param) {
    Lval* last_expr = eval_list_but_last(bindings_env, lval_fun->body);
    if (!last_expr) return make_lval_list();
    if (last_expr->type == LVAL_ERR) return last_expr;

    last_expr->tco_env = retain(bindings_env);
    return last_expr;
  } else {
    Lval* params = make_lval_vector();
    params->head = retain(iter_current_cell(p));
    return make_lval_lambda(retain(bindings_env), params,
                            retain(lval_fun->body), LAMBDA);
  }
}

Lval* expand_macro(Lval* lval_fun, Lval* arg_list) {
  Lval* lval = eval_lambda_call(lval_fun, arg_list);

  if (lval->type == LVAL_FUNCTION)
    return make_lval_err("Macro needs all its params bound");
  if (lval->type == LVAL_ERR) return lval;

  Lenv* bindings_env = lval->tco_env;
  lval->tco_env = NIL;
  return lval_eval(bindings_env, lval);
}

Lval* eval_macro_call(Lenv* env, Lval* lval_fun, Lval* arg_list) {
  Lval* expanded_macro = expand_macro(lval_fun, arg_list);
  if (expanded_macro->type == LVAL_ERR) return expanded_macro;

  // Expanded macro closes over the environment where it is executed
  return lval_eval(env, expanded_macro);

  /* expanded_macro->tco_env = env;  */
  /* return expanded_macro; */
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

  if (lval_list->head == NIL) {
    return make_lval_list();  // empty;
  }
  Lval* lval_first = lval_list->head->car;
  scoped Lval* lval_fun = lval_eval(env, lval_first);

  if (lval_fun->type == LVAL_ERR) return retain(lval_fun);

  if (lval_fun->type != LVAL_FUNCTION)
    return make_lval_err(
        "sexpr starts with incorrect type. "
        "Got %s, expected %s.",
        lval_type_to_name(lval_fun), lval_type_constant_to_name(LVAL_FUNCTION));

  /* Lval* ret = NIL; */
  scoped Lval* arg_list = make_lval_list();
  arg_list->head = list_rest(lval_list->head);
  scoped Lval* evalled_arg_list = NIL;
  switch (lval_fun->subtype) {
    case SYS:
    case LAMBDA:
      evalled_arg_list = eval_nodes(env, arg_list);
      if (evalled_arg_list->type == LVAL_ERR) return retain(evalled_arg_list);
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
      return eval_lambda_call(lval_fun, evalled_arg_list);
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
      case LVAL_ERR:
        // A tco might be calling eval on an error.
        return lval;
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
