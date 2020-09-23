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

Lval* read_rest_args(Lval* param, Cell* p, Cell* args) {
  Lval* rest_args;
  if (args) {
    rest_args = make_lval_list();
    rest_args->head = retain(args);
  } else
    rest_args = make_lval_nil();
  return rest_args;
}

Lval* eval_lambda_call(Lval* lval_fun, Lval* arg_list) {
  /* printf("---------------------eval_lambda_call %d %d\n",
   * lval_fun->param_count, */
  /*        lval_fun->rest_arg_index); */
  /* printf("namespace: %s\n ", lval_fun->ns->namespace); */
  /* lval_println(lval_fun); */

  int rest_arg_index = lval_fun->rest_arg_index;  // 1 based
  int param_count = lval_fun->param_count;
  int min_param_count = rest_arg_index ? param_count - 1 : param_count;
  int arg_list_count =
      list_count(lval_fun->partials) + list_count(arg_list->head);
  if (arg_list_count < min_param_count)
    return make_lval_err("Not enough args passed, expected %d, got %d.",
                         min_param_count, arg_list_count);
  if (!rest_arg_index && arg_list_count > param_count)
    return make_lval_err("Too many args passed, expected %i, got %i.",
                         param_count, arg_list_count);

  scoped Lenv* bindings_env = lenv_new();

  scoped_iter Cell* p = iter_new(lval_fun->params);
  Lval* param = iter_next(p);

  scoped Cell* head = list_concat(lval_fun->partials, arg_list->head);

  int i = 0;
  while (i < min_param_count) {
    Lval* arg = head->car;
    bindings_env->kv =
        alist_prepend(bindings_env->kv, retain(param), retain(arg));
    head = head->cdr;
    param = iter_next(p);
    i++;
  }

  if (rest_arg_index) {
    param = iter_next(p);
    /* Lval* rest_args = read_rest_args(param, p, args); */
    scoped Lval* rest_arg = make_lval_list();
    Cell** cdr = &rest_arg->head;
    while (head) {
      Lval* arg = head->car;
      Cell* c = make_cell();
      c->car = retain(arg);
      *cdr = c;
      cdr = &c->cdr;
      head = head->cdr;
    }
    if (!rest_arg->head) {
      rest_arg->type = LVAL_LITERAL;
      rest_arg->subtype = LNIL;
    }
    /*   retain(args); */
    /* lval_println(rest_args); */
    /* printf("ref count: %d\n", get_ref_count(args)); */
    bindings_env->kv =
        alist_prepend(bindings_env->kv, retain(param), retain(rest_arg));
  }

  bindings_env->parent_env = retain(lval_fun->closure);
  /* env_print(bindings_env); */
  /* printf("------\n"); */

  Namespace* old_ns = state->current_ns;
  // TODO: bit iffy, look into more
  if (lval_fun->ns) state->current_ns = lval_fun->ns;
  Lval* ret = do_list(bindings_env, lval_fun->body, RETURN_ON_ERROR);

  state->current_ns = old_ns;
  return ret;
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
  Namespace* current_ns = get_current_ns();
  // From a required namespace
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
    ns->dependants = alist_put(ns->dependants, is_eq_str,
                               retain(current_ns->namespace), current_ns);
    ret.lval->ns = ns;
    return ret;
  }

  // From the current namespace
  lval_resolved_sym = lenv_get(env, lval_symbol);
  if (lval_resolved_sym) {
    ret.lval = lval_resolved_sym;  // resolved in symbols lexical env
    ret.lval->ns = current_ns;
    return ret;
  }

  // From a required namespace
  Namespace* ns = get_ns_for_referred_symbol(lval_symbol);
  if (ns) {
    /* ret.env = ns->env; */
    ret.ns = ns;
    ret.name = retain(lval_symbol->str);
    /* ret.fqn = make_fqn(ns->namespace, lval_symbol->str); */
    ret.lval =
        lenv_get_or_error(ns->env,
                          lval_symbol);  // resolved as a referring symbol

    ns->dependants = alist_put(ns->dependants, is_eq_str,
                               retain(current_ns->namespace), current_ns);

    ret.lval->ns = ns;
    return ret;
  }

  // From stdlib
  ns = state->stdlib_ns;
  if (ns) {
    lval_resolved_sym = lenv_get(ns->env, lval_symbol);
    if (lval_resolved_sym) {
      ret.lval = lval_resolved_sym;  // resolved in stdlib env
      /* ret.fqn = make_fqn(ns->namespace, lval_symbol->str); */
      ret.ns = ns;
      ret.lval->ns = ns;
      return ret;
    }
  }

  // From builtins
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
Lval* eval_application(Lenv* env, Lval* lval_list) {
  /* printf("evalling fn call: \n"); */
  /* lval_println(lval_list); */

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
    case SYS:;
      Cell* head = list_concat(lval_fun->partials, evalled_arg_list->head);
      Lval* rest_arg = make_lval_list();
      Cell** cdr = &rest_arg->head;
      while (head) {
        Lval* arg = head->car;
        Cell* c = make_cell();
        c->car = retain(arg);
        *cdr = c;
        cdr = &c->cdr;
        head = head->cdr;
      }
      ret = lval_fun->fun(env, rest_arg);
      release(head);
      release(rest_arg);
      return ret;
    case SPECIAL: {
      return lval_fun->fun(env, arg_list);
    }
    case MACRO:
      return eval_macro_call(env, lval_fun, arg_list);
    case LAMBDA:
      return eval_lambda_call(lval_fun, evalled_arg_list);
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
          return eval_application(env, lval);
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
