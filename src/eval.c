#include "eval.h"

#include "assert.h"
#include "cell.h"
#include "env.h"
#include "io.h"
#include "iter.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "misc_fns.h"
#include "namespace.h"
#include "print.h"
#include "read.h"
#include "state.h"

Lval* lval_eval(Lenv* e, Lval* v);

Lval* map_eval(Lenv* env, Lval* lval_list) {
  debug("eval nodes, new list:");
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

Lval* do_list(Lenv* env, Lval* list, int mode) {
  scoped_iter Cell* i = iter_new(list);
  Lval* lval = iter_next(i);
  Lval* result = NIL;
  while (lval) {
    if (iter_peek(i)) {
      result = lval_eval(env, lval);
      if (result->type == LVAL_ERR) {
        lval_println(lval);
        lval_println(result);
        if (mode == RETURN_ON_ERROR) return result;
      }
      release(result);
    } else {
      result = lval_eval(env, lval);
      if (result->type == LVAL_ERR) {
        lval_print(lval);
        printf(" resulted in: ");
        lval_println(result);
      }
      return result;
    }
    lval = iter_next(i);
  }
  return make_lval_nil();
}

Lval* read_rest_args(Lval* param, Cell* p, Cell* a) {
  /* if the param is & there should be exactly one formal left  */
  if (!param || iter_peek(p)) {
    return NIL;
  }
  Lval* rest_args = make_lval_list();
  rest_args->head = retain(iter_current_cell(a));
  Lval* arg = NIL;
  do arg = iter_next(a);
  while (arg);
  return rest_args;
}

Lval* eval_lambda_call(Lval* lval_fun, Lval* arg_list) {
  debug("---------------------eval_lambda_call\n");
  scoped Lenv* bindings_env = lenv_new();

  scoped_iter Cell* p = iter_new(lval_fun->params);
  Lval* param = iter_next(p);

  scoped_iter Cell* a = iter_new(arg_list);
  Lval* arg = iter_next(a);

  // Try to bind all the arg in arg_list ====================
  while (param) {
    if (_strcmp(param->str, "&") == 0) {
      param = iter_next(p);
      arg = read_rest_args(param, p, a);
      if (!arg)
        return make_lval_err(
            "Function format invalid. "
            "Symbol '&' not followed by single symbol.");
    } else
      retain(arg);

    if (arg)
      bindings_env->kv = alist_prepend(bindings_env->kv, retain(param), arg);
    else
      break;
    arg = iter_next(a);
    param = iter_next(p);
  }
  if (!param && arg)
    return make_lval_err("Function passed too many args. Got %i, expected %i.",
                         list_count(arg_list->head),
                         list_count(lval_fun->params->head));

  bindings_env->parent_env = retain(lval_fun->closure);

  /* Eval body expressions, but only if all params are bound */
  if (!param) {
    return do_list(bindings_env, lval_fun->body, RETURN_ON_ERROR);
  } else {
    Lval* params = make_lval_vector();
    params->head = retain(iter_current_cell(p));
    return make_lval_lambda(retain(bindings_env), params,
                            retain(lval_fun->body), LAMBDA);
  }
}

Lval* expand_macro(Lval* lval_fun, Lval* arg_list) {
  debug(">>>>>>>>>>>>>>>>>>> expand_macro:\n ");
  lval_debugln(lval_fun);
  Lval* lval = eval_lambda_call(lval_fun, arg_list);
  if (lval->type == LVAL_FUNCTION) {
    release(lval);
    return make_lval_err("Macro needs all its params bound");
  }
  lval_debugln(lval);

  return lval;
}

Lval* eval_macro_call(Lenv* env, Lval* lval_fun, Lval* arg_list) {
  scoped Lval* expanded_macro = expand_macro(lval_fun, arg_list);
  if (expanded_macro->type == LVAL_ERR) return retain(expanded_macro);

  // Expanded macro closes over the environment where it is executed
  return lval_eval(env, expanded_macro);
}

struct resolved_symbol eval_symbol(Lenv* env, Lval* lval_symbol) {
  struct resolved_symbol ret = {};
  Lval* lval_resolved_sym;
  scoped char* namespace_or_alias = get_namespace_part(lval_symbol);
  if (namespace_or_alias) {
    Namespace* ns =
        get_ns_for_namespaced_symbol(lval_symbol, namespace_or_alias);
    if (!ns) {
      ret.lval =
          make_lval_err("Can't find namespace to resolve %s", lval_symbol->str);
      return ret;
    }
    scoped char* name = get_name_part(lval_symbol);
    scoped Lval* lval_name = make_lval_sym(name);
    ret.ns = ns;
    ret.name = retain(name);
    ret.lval =
        lenv_get_or_error(ns->env,
                          lval_name);  // resolved in the symbol's namespace

    /* ret.fn_table_index_global = make_fqn("fn:", ns->namespace, name); */
    /* ret.data_ptr_global = make_fqn("data:", ns->namespace, name); */
    return ret;
  }

  lval_resolved_sym = lenv_get(env, lval_symbol);
  if (lval_resolved_sym) {
    ret.lval = lval_resolved_sym;  // resolved in symbols lexical env
    return ret;
  }
  Namespace* ns = get_ns_for_referred_symbol(lval_symbol);
  if (ns) {
    /* ret.env = ns->env; */
    ret.ns = ns;
    ret.name = retain(lval_symbol->str);
    /* ret.fqn = make_fqn(ns->namespace, lval_symbol->str); */
    ret.lval =
        lenv_get_or_error(ns->env,
                          lval_symbol);  // resolved as a referring symbol
    return ret;
  }

  ns = state->stdlib_ns;
  if (ns) {
    lval_resolved_sym = lenv_get(ns->env, lval_symbol);
    if (lval_resolved_sym) {
      ret.lval = lval_resolved_sym;  // resolved in stdlib env
      /* ret.fqn = make_fqn(ns->namespace, lval_symbol->str); */
      ret.ns = ns;
      return ret;
    }
  }

  Lenv* builtins_env = state->builtins_env;
  ret.lval = lenv_get_or_error(builtins_env,
                               lval_symbol);  // resolved in builtins env
  return ret;
}

Lval* eval_vector(Lenv* env, Lval* lval_vector) {
  lval_vector = map_eval(env, lval_vector);
  lval_vector->subtype = VECTOR;
  return lval_vector;
}

// We've got a list. We expect first node to be a fn call, and the rest args to
// the fn.
Lval* eval_list(Lenv* env, Lval* lval_list) {
  debug("evalling fn call: ");
  lval_debugln(lval_list);

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

  Lval* ret = NIL;
  scoped Lval* arg_list = make_lval_list();
  arg_list->head = list_rest(lval_list->head);
  scoped Lval* evalled_arg_list = NIL;
  switch (lval_fun->subtype) {
    case SYS:
    case LAMBDA:
      evalled_arg_list = map_eval(env, arg_list);
      if (evalled_arg_list->type == LVAL_ERR) return retain(evalled_arg_list);
    default:;
  }
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
      return ret;
      break;
    case LAMBDA:
      return eval_lambda_call(lval_fun, evalled_arg_list);
      break;
    default:
      return make_lval_err("Unknown fun subtype %d for %s", lval_fun->subtype,
                           lval_fun->str);
  }
}

Lval* lval_eval(Lenv* env, Lval* lval) {
  /* info("\n->>>>>>>>EVAL: "); */
  /* lval_infoln(lval); */
  /* printf("lval->type: %s\n", lval_type_constant_to_name(lval->type)); */
  /* Lval* ret = NIL; */
  switch (lval->type) {
    case LVAL_SYMBOL:
      return eval_symbol(env, lval).lval;
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:
          return eval_list(env, lval);
        case VECTOR:
          return eval_vector(env, lval);
        case MAP:
          /* TODO: */
          return lval;
        default:
          return make_lval_err("Unknown seq subtype: %d ", lval->subtype);
      }
      break;
    default:
      return retain(lval);
  }
}
