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
  Cell** lp = &(new_list->data.head);
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
    rest_args->data.head = retain(args);
  } else
    rest_args = make_lval_nil();
  return rest_args;
}

Lval* eval_lambda_call(Lval* lval_fn, Lval* arg_list) {
  int arg_count =
      list_count(arg_list->data.head) + list_count(lval_fn->partials);
  int arity = min(arg_count, MAX_FN_PARAMS);
  Lambda* lambda = lval_fn->lambdas[arity];
  /* for (int i = 0; i <= MAX_FN_PARAMS; i++) */
  /*   printf("%d: %p\n", i, lval_fn->lambdas[i]); */
  if (!lambda)
    return make_lval_err("Wrong number of args (%d) passed to %s", arity,
                         lval_fn->cname);

  int rest_arg_index = lambda->has_rest_arg;  // 1 based
  int param_count = lambda->param_count;
  int min_param_count = rest_arg_index ? param_count - 1 : param_count;

  scoped Lenv* bindings_env = lenv_new();

  scoped_iter Cell* p = iter_new(lambda->params);
  Lval* param = iter_next(p);

  scoped Cell* head = list_concat(lval_fn->partials, arg_list->data.head);

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
    scoped Lval* rest_arg = make_lval_list();
    Cell** cdr = &rest_arg->data.head;
    while (head) {
      Lval* arg = head->car;
      Cell* c = make_cell();
      c->car = retain(arg);
      *cdr = c;
      cdr = &c->cdr;
      head = head->cdr;
    }
    if (!rest_arg->data.head) {
      rest_arg->type = LVAL_LITERAL;
      rest_arg->subtype = LNIL;
    }
    bindings_env->kv =
        alist_prepend(bindings_env->kv, retain(param), retain(rest_arg));
  }

  bindings_env->parent_env = retain(lval_fn->closure);

  Namespace* old_ns = state->current_ns;
  // TODO: bit iffy, look into more
  if (lval_fn->ns) state->current_ns = lval_fn->ns;
  Lval* ret = do_list(bindings_env, lambda->body, RETURN_ON_ERROR);

  state->current_ns = old_ns;
  /* Lval* ret = do_list(bindings_env, lambda->body, RETURN_ON_ERROR); */

  return ret;
}

Lval* expand_macro(Lval* lval_fn, Lval* arg_list) {
  Lval* lval = eval_lambda_call(lval_fn, arg_list);
  return lval;
}

Lval* eval_macro_call(Lenv* env, Lval* lval_fun, Lval* arg_list) {
  scoped Lval* expanded_macro = expand_macro(lval_fun, arg_list);
  if (expanded_macro->type == LVAL_ERR) return retain(expanded_macro);

  // Expanded macro closes over the environment where it is executed
  return lval_eval(env, expanded_macro);
}

Lval* eval_symbol(Lenv* env, Lval* lval_symbol) {
  Lval* lval_resolved_sym;
  scoped char* namespace_or_alias = get_namespace_part(lval_symbol);
  Namespace* current_ns = get_current_ns();
  if (_strcmp(lval_symbol->data.str, "foox") == 0) {
    printf("eval symbol: %s\n", lval_symbol->data.str);
    printf("Env:\n");
    env_print(env);
    printf("Parent env:\n");
    env_print(env->parent_env);
    printf("Grandparent env:\n");
    if (env->parent_env) env_print(env->parent_env->parent_env);
  }
  // From a required namespace
  if (namespace_or_alias) {
    /* printf("eval_symbol: %s\n", lval_symbol->data.str); */
    Namespace* ns =
        get_ns_for_namespaced_symbol(lval_symbol, namespace_or_alias);
    if (!ns) {
      return make_lval_err("Can't find namespace to resolve %s",
                           lval_symbol->data.str);
    }
    scoped char* name = get_name_part(lval_symbol);
    scoped Lval* lval_name = make_lval_sym(name);
    Lval* lval =
        lenv_get_or_error(ns->env,
                          lval_name);  // resolved in the symbol's namespace
    if (state->mark_deps)
      ns->dependants = alist_put(ns->dependants, is_eq_str,
                                 retain(current_ns->namespace), current_ns);
    return lval;
  }

  // From the current env
  lval_resolved_sym = lenv_get(env, lval_symbol);
  if (lval_resolved_sym) {
    return lval_resolved_sym;  // resolved in symbols lexical env
  }

  // From a required namespace
  Namespace* ns = get_ns_for_referred_symbol(lval_symbol);
  if (ns) {
    Lval* lval =
        lenv_get_or_error(ns->env,
                          lval_symbol);  // resolved as a referring symbol

    if (state->mark_deps)
      ns->dependants = alist_put(ns->dependants, is_eq_str,
                                 retain(current_ns->namespace), current_ns);

    /* ret.lval->ns = ns; */
    return lval;
  }

  // From stdlib (wajure.core)
  ns = state->stdlib_ns;
  if (ns) {
    lval_resolved_sym = lenv_get(ns->env, lval_symbol);
    if (lval_resolved_sym) {
      return lval_resolved_sym;  // resolved in stdlib env
    }
  }

  // From builtins (c functions)
  Lenv* builtins_env = state->builtins_env;
  return lenv_get_or_error(builtins_env,
                           lval_symbol);  // resolved in builtins env
}

Lval* eval_vector(Lenv* env, Lval* lval_vector) {
  lval_vector = map_eval(env, lval_vector);
  lval_vector->subtype = VECTOR;
  return lval_vector;
}

// We've got a list. We expect first node to be a fn call, and the rest args to
// the fn.
Lval* eval_application(Lenv* env, Lval* lval_list) {
  if (lval_list->data.head == NIL) {
    return make_lval_list();  // empty;
  }
  Lval* lval_first = lval_list->data.head->car;
  scoped Lval* lval_fun = lval_eval(env, lval_first);

  if (lval_fun->type == LVAL_ERR) return retain(lval_fun);

  if (lval_fun->type != LVAL_FUNCTION)
    return make_lval_err(
        "sexpr starts with incorrect type. "
        "Got %s, expected %s.",
        lval_type_to_name(lval_fun), lval_type_constant_to_name(LVAL_FUNCTION));

  Lval* ret = NIL;
  scoped Lval* arg_list = make_lval_list();
  arg_list->data.head = list_rest(lval_list->data.head);
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
      Cell* head = list_concat(lval_fun->partials, evalled_arg_list->data.head);
      Lval* rest_arg = make_lval_list();
      Cell** cdr = &rest_arg->data.head;
      while (head) {
        Lval* arg = head->car;
        Cell* c = make_cell();
        c->car = retain(arg);
        *cdr = c;
        cdr = &c->cdr;
        head = head->cdr;
      }
      ret = lval_fun->c_fn(env, rest_arg);
      release(head);
      release(rest_arg);
      return ret;
    case SPECIAL: {
      return lval_fun->c_fn(env, arg_list);
    }
    case MACRO:
      return eval_macro_call(env, lval_fun, arg_list);
    case LAMBDA:
      return eval_lambda_call(lval_fun, evalled_arg_list);
    default:
      return make_lval_err("Unknown fun subtype %d for %s", lval_fun->subtype,
                           lval_fun->data.str);
  }
}

Lval* lval_eval(Lenv* env, Lval* lval) {
  /* info("\n->>>>>>>>EVAL: "); */
  /* lval_infoln(lval); */
  /* printf("lval->type: %s\n", lval_type_constant_to_name(lval->type)); */
  /* Lval* ret = NIL; */
  switch (lval->type) {
    case LVAL_SYMBOL:
      return eval_symbol(env, lval);
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
