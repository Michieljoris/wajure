#include "assert.h"
#include "cell.h"
#include "env.h"
#include "eval.h"
#include "fns.h"
#include "io.h"
#include "iter.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "misc_fns.h"
#include "namespace.h"
#include "print.h"
#include "state.h"

Lval* eval_quote(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("quote", 1);
  ITER_NEXT;
  Lval* ret = arg;
  ITER_END;
  retain(ret);
  return ret;
}

char* make_cname(char* name) {
  int uid = get_current_ns()->uid_counter++;
  int uid_length = number_len(uid);
  char* cname = lalloc_size(_strlen(name) + uid_length + 1);
  sprintf(cname, "%s_%d", name, uid);
  return cname;
}

void bind_symbol(Lval* lval_sym, Lval* lval) {
  Namespace* ns = get_current_ns();
  // We want any fn that's defined too have only access to the env as defined
  // sofar.
  /* Lenv* env = lenv_new(); */
  /* env->kv = retain(ns->env->kv); */
  /* Lval* bound_symbol = eval_symbol(env, lval_sym); */
  /* release(env); */
  /* /\* printf("bound_symbol:"); *\/ */
  /* /\* lval_println(bound_symbol); *\/ */
  /* if (bound_symbol->group != LVAL_ERR) { */
  /*   if (bound_symbol->type == SYS) { */
  /*     warn("WARNING: %s already refers to a builtin: ", lval_sym->data.str);
   */
  /*     lval_println(bound_symbol); */
  /*   } else */
  /*     warn("WARNING: %s already refers to: #'%s/%s in namespace: %s\n", */
  /*          lval_sym->data.str, bound_symbol->ns->namespace,
   * lval_sym->data.str, */
  /*          bound_symbol->ns->namespace); */
  /*   release(bound_symbol); */
  /* } */
  if (!lval->cname) {
    // We need to keep track of in which namespace a fn is actually defined and
    // under which name so we can refer to its fn_table_index from other
    // namespaces in compiled code by imported global.

    /* printf("def of lambda: %s/%s\n", get_current_ns()->namespace, */
    /*        lval_sym->data.str); */
    /* lval_println(lval); */

    // Set a unique cname on every lval we add to our environment
    lval->cname = make_cname(lval_sym->data.str);
    lval->ns = retain(get_current_ns());
  }
  // We want to mutate env as we update it as this is a namespace root binding.
  lenv_put(ns->env, lval_sym, lval);
}

Lval* eval_def(Lenv* env, Lval* arg_list) {
  ITER_NEW("def");
  ITER_NEXT_TYPE(LVAL_LITERAL, SYMBOL);
  Lval* lval_sym = arg;

  Namespace* ns = get_current_ns();
  Lval* bound_symbol = eval_symbol(ns->env, lval_sym);
  /* printf("bound_symbol:"); */
  /* lval_println(bound_symbol); */
  Lval* lval_unbound = NULL;
  if (bound_symbol->group == LVAL_ERR) {
    lval_unbound = make_lval_unbound();
    lenv_put(ns->env, lval_sym, lval_unbound);
  } else {
    if (bound_symbol->type == SYS) {
      warn("WARNING: %s already refers to a builtin: ", lval_sym->data.str);
      lval_println(bound_symbol);
    } else
      warn("WARNING: %s already refers to: #'%s/%s in namespace: %s\n",
           lval_sym->data.str, bound_symbol->ns->namespace, lval_sym->data.str,
           bound_symbol->ns->namespace);
  }
  release(bound_symbol);

  /* printf("\n++++++++++++in eval_def\n"); */
  /* lval_println(lval_sym); */
  ITER_NEXT;
  Lval* lval = arg;
  /* lval_println(lval); */
  ITER_END;
  if (lval) {
    // We want any fn that's defined to have only access to the env as defined
    // sofar.
    /* Lenv* env = lenv_new(); */
    /* env->kv = retain(ns->env->kv); */
    lval = lval_eval(env, lval);
    /* release(env); */
    if (lval->group == LVAL_ERR) return lval;

    if (!lval->cname) {
      // We need to keep track of in which namespace a fn is actually defined
      // and under which name so we can refer to its fn_table_index from other
      // namespaces in compiled code by imported global.

      /* printf("def of lambda: %s/%s\n", get_current_ns()->namespace, */
      /*        lval_sym->data.str); */
      /* lval_println(lval); */

      // Set a unique cname on every lval we add to our environment
      lval->cname = make_cname(lval_sym->data.str);
      lval->ns = retain(get_current_ns());
    }
    // We want to mutate env as we update it as this is a namespace root
    // binding.
    lenv_put(ns->env, lval_sym, lval);
    release(lval);
  }

  return make_lval_nil();
}

Lval* eval_if(Lenv* env, Lval* arg_list) {
  ITER_NEW_MIN_MAX("if", 2, 3);
  ITER_NEXT;
  scoped Lval* cond = lval_eval(env, arg);
  if (cond->group == LVAL_ERR) return retain(cond);
  Lval* branch = NIL;
  if (cond->type != LFALSE && cond->type != LNIL) { /* TRUE */
    ITER_NEXT;
    branch = arg;
    ITER_NEXT;
  } else { /* FALSE  */
    ITER_NEXT;
    ITER_NEXT;
    branch = arg;
  }
  ITER_END;
  if (branch) {
    return lval_eval(env, branch);
  }
  return make_lval_nil();
}

int is_unique_param(Lval* lval_sym, Cell* param_list) {
  Cell* head = param_list;
  char* sym_str = lval_sym->data.str;
  while (head) {
    Lval* param = head->car;
    if (_strcmp(sym_str, param->data.str) == 0) return 0;
    head = head->cdr;
  }
  return 1;
}

Lambda* eval_lambda_form(Lenv* env, Lval* lval_list) {
  if (lval_list->type != LIST)
    return make_lambda_err(
        make_lval_err("Expecting list to hold params and body"));

  Cell* head = lval_list->data.head;
  if (!head || ((Lval*)head->car)->type != VECTOR)
    return make_lambda_err(
        make_lval_err("lambda expects at least a param vector"));

  Lval* lval_params = head->car;
  Cell* param = lval_params->data.head;
  // Includes any rest arg, so [a & b] has param_count = 2
  int param_count = 0;
  int rest_arg_index = 0;
  int offset = 0;
  while (param) {
    if (((Lval*)param->car)->type != SYMBOL) {
      return make_lambda_err(make_lval_err(
          "Canot bind non-symbol. Got %s, expected %s.",
          lval_type_to_name(param->car), lval_type_constant_to_name(SYMBOL)));
    }
    if (rest_arg_index && param_count == rest_arg_index) {
      return make_lambda_err(
          make_lval_err("Function format invalid. "
                        "Symbol '&' not followed by single symbol."));
    }
    Lval* lval_sym = param->car;
    if (_strcmp(lval_sym->data.str, "&") == 0) {
      rest_arg_index = offset + 1;  // number of params
      param = param->cdr;
      continue;
    }

    if (!is_unique_param(lval_sym, param->cdr)) {
      return make_lambda_err(
          make_lval_err("param %s is not unique", lval_sym->data.str));
    }
    offset++;

    param_count++;
    param = param->cdr;
  }

  if (rest_arg_index && param_count != rest_arg_index)
    return make_lambda_err(
        make_lval_err("Function format invalid. "
                      "Symbol '&' not followed by single symbol."));

  if (param_count > MAX_FN_PARAMS)
    return make_lambda_err(
        make_lval_err("A function cannot have more than 20 parameters"));

  Lval* lval_body = make_lval_list();
  lval_body->data.head = list_rest(lval_list->data.head);
  Lambda* lambda =
      make_lambda(retain(lval_params), param_count, rest_arg_index, lval_body);
  return lambda;
}

// Processes list of lambdas
Lval* eval_lambdas(Lenv* env, Cell* arg_list, int type) {
  Cell* head = arg_list;
  int s = (MAX_FN_PARAMS + 1) * (sizeof(char*));
  Lambda** lambdas = lalloc_size(s);
  memset(lambdas, 0, s);

  int max_param_count = 0;
  int has_rest_arg = 0;
  Lval* err = NULL;
  Lval* fn_name = NULL;
  if (head->car) {
    Lval* first_arg = ((Lval*)head->car);
    /* printf("-------------------\n"); */
    /* lval_println(first_arg); */
    if (first_arg->type == SYMBOL) {
      fn_name = first_arg;
      head = head->cdr;
    }
  }
  do {
    Lambda* lambda = eval_lambda_form(env, head->car);
    if (lambda->err) {
      err = lambda->err;
      break;
    }
    int param_count = lambda->param_count;
    if (lambda->has_rest_arg) {
      if (has_rest_arg) {
        err = make_lval_err("Can't have more than 1 variadic overload");
        break;
      }
      has_rest_arg = lambda->has_rest_arg;
      int from = lambdas[has_rest_arg - 1] ? has_rest_arg : has_rest_arg - 1;
      for (int i = from; i <= MAX_FN_PARAMS; i++) lambdas[i] = retain(lambda);
    } else {
      if (lambdas[param_count]) {
        Lambda* lambda = lambdas[param_count];
        if (!has_rest_arg || param_count < has_rest_arg - 1 ||
            (param_count == has_rest_arg - 1 && !lambda->has_rest_arg)) {
          err = make_lval_err("Can't have 2 overloads with same arity");
          break;
        }
      }
      max_param_count = max(max_param_count, param_count);
      lambdas[param_count] = retain(lambda);
    }
    if (has_rest_arg && max_param_count >= has_rest_arg) {
      err = make_lval_err(
          "Can't have fixed arity function with more params than variadic "
          "function");
      break;
    }

    head = head->cdr;
    /* printf("lambda param_count: %d, has_rest_arg: %d\n", lambda->param_count,
     */
    /*        lambda->has_rest_arg); */

    /* for (int i = 0; i <= MAX_FN_PARAMS; i++) printf("%d: %p, ", i,
     * lambdas[i]); */
    /* printf("\n"); */
  } while (head);

  if (err) {
    for (int i = 0; i <= MAX_FN_PARAMS; i++) release(lambdas[i]);
    return err;
  }

  Lval* fn = make_lval_lambda(retain(env), type, lambdas);
  if (fn_name) {
    // The fn can be found in its own closure
    fn->closure = lenv_prepend(env, retain(fn_name), retain(fn));
    fn->data.str = retain(fn_name->data.str);
  }

  /* printf("!!!!!!!!!!!!!!!! %p\n", env); */
  /* printf("closure_env of eval_lambda: \n"); */
  /* printf("env:\n"); */
  /* env_print(env); */
  /* /\* /\\* lval_println(fn); *\\/ *\/ */
  /* printf("closure_env:\n"); */
  /* env_print(fn->closure); */
  /* printf("closure_env parent_env:\n"); */
  /* env_print(fn->closure->parent_env); */
  /* printf("closure_env parent parent_env:\n"); */
  /* if (fn->closure->parent_env)
   * env_print(fn->closure->parent_env->parent_env); */
  /* printf("!!!!!!!!!!!!!!!!\n"); */
  return fn;
}

// fn*
Lval* eval_lambda(Lenv* env, Lval* arg_list) {
  Cell* head = arg_list->data.head;
  if (!head) return make_lval_err("fn* expects at least one arg");
  return eval_lambdas(env, head, LAMBDA);
}

// Macros close over the environment where they are defined.
Lval* eval_macro(Lenv* env, Lval* arg_list) {
  Cell* head = arg_list->data.head;
  if (!head) return make_lval_err("macro expects at least one arg");
  return eval_lambdas(env, head, MACRO); /*  */
}

int is_fn_call(Lval* lval, char* sym, int min_node_count) {
  if (lval->group == LVAL_COLLECTION && lval->type == LIST &&
      list_count(lval->data.head) >= min_node_count) {
    lval = lval->data.head->car;
    return lval->type == SYMBOL && _strcmp(lval->data.str, sym) == 0;
  }
  return 0;
}

Lval* eval_unquote(Lenv* env, Lval* lval) {                     /*  */
  LASSERT_LIST_COUNT(lval, lval->data.head->cdr, 1, "unquote"); /*  */
  return lval_eval(env, lval->data.head->cdr->car);             /*  */
} /*  */
/*  */
Lval* eval_splice_unquote(Lenv* env, Lval* lval) {                     /*  */
  LASSERT_LIST_COUNT(lval, lval->data.head->cdr, 1, "splice-unquote"); /*  */
  Lval* ret = lval_eval(env, lval->data.head->cdr->car);               /*  */
  if (ret->group == LVAL_ERR) return ret;                              /*  */
  if (ret->type == LNIL) return ret;                                   /*  */
  LASSERT_LVAL_IS_LIST_TYPE(ret, "splice-unquote");                    /*  */
  return ret;                                                          /*  */
} /*  */
/*  */
Lval* eval_quasiquote_nodes(Lenv* env, Lval* arg_list) { /*  */
  scoped_iter Cell* i = iter_new(arg_list);              /*  */
  Lval* arg = iter_next(i);                              /*  */
                                                         /*  */
  /* a place to store processed cells */
  Lval* evalled_list = make_lval_list();
  Cell** lp;
  lp = &evalled_list->data.head;
  Cell* new_cell;
  while (arg) {
    /* Take the first of the remaining nodes*/
    if (is_fn_call(arg, "unquote", 2)) {
      /* Eval if node is unquoted */
      Lval* unquoted_lval = eval_unquote(env, arg);
      if (unquoted_lval->group == LVAL_ERR) {
        release(evalled_list);
        return unquoted_lval;
      }

      /* And add to processed nodes */
      new_cell = make_cell();
      new_cell->car = unquoted_lval;
      *lp = new_cell;
      lp = &(new_cell->cdr);

    } else if (is_fn_call(arg, "splice-unquote", 2)) {
      /* Eval if node is splice unquoted */
      Lval* splice_unquoted_eval = eval_splice_unquote(env, arg);
      if (splice_unquoted_eval->group == LVAL_ERR) {
        release(evalled_list);
        return splice_unquoted_eval;
      }
      /* And concat to processed nodes */
      *lp = retain(splice_unquoted_eval->data.head);
      release(splice_unquoted_eval);
      Cell* last_cell = list_last(splice_unquoted_eval->data.head);
      release(last_cell);  //???????
      lp = &(last_cell->cdr);
    } else {
      /* if node is a list apply quasiquote recursively */
      if (arg->group == LVAL_COLLECTION) {
        int type = arg->type;
        arg = eval_quasiquote_nodes(env, arg);
        if (arg->group == LVAL_ERR) {
          release(evalled_list);
          return arg;
        }
        arg->type = type;
      } else {
        retain(arg);
      }

      new_cell = make_cell();
      new_cell->car = arg;
      *lp = new_cell;
      lp = &(new_cell->cdr);
    }
    arg = iter_next(i);
  }
  /* printf("========================================\n"); */
  /* lval_println(evalled_list); */
  /* printf("========================================\n"); */
  return evalled_list;
}

/* Symbols/Lists/Vectors/Sets/Maps */
Lval* eval_quasiquote(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("quasiquote", 1)
  ITER_NEXT
  Lval* qq_arg = arg;
  ITER_END;
  Lval* ret = NIL;
  switch (qq_arg->group) {
    case LVAL_COLLECTION:
      switch (qq_arg->type) {
        case LIST:
          if (is_fn_call(qq_arg, "unquote", 2)) {
            ret = eval_unquote(env, qq_arg);
            break;
          }
          LASSERT(arg_list, !is_fn_call(qq_arg, "splice-unquote", 2),
                  "Trying to splice unquote a %s. Can only use splice "
                  "unquote in a "
                  "quasiquoted list",
                  lval_type_to_name(qq_arg->data.head->cdr->car));
          ret = eval_quasiquote_nodes(env, qq_arg);
          break;
        case VECTOR:
          ret = eval_quasiquote_nodes(env, qq_arg);
          ret->type = VECTOR;
          break;
        case MAP:

          /* todo  */
          ret = qq_arg;
      }
      break;
    default: /* symbols, strings, numbers etc */
      ret = retain(qq_arg);
  }
  return ret;
}

/* Implemented as a little state machine */
enum { EXPR, CATCH, FINALLY };
Lval* eval_try(Lenv* env, Lval* arg_list) {
  ddebug("in eval_try\n");
  scoped_iter Cell* i = iter_new(arg_list);
  Lval* arg = iter_next(i);
  int mode = EXPR;
  Lval* ret = make_lval_list();
  Lval* node;
  while (arg) {
    node = arg;
    lval_debugln(node);
    switch (mode) {
      case EXPR:
        ddebug("in EXPR\n");
        /* CATCH node =================== */
        if (is_fn_call(node, "catch", 1)) {
          debug("from EXPR, found catch\n");
          mode = CATCH;
          if (ret->group == LVAL_ERR) {
            if (list_count(node->data.head) < 4) {
              release(ret);
              return make_lval_err(
                  "catch clause should have at least an exception type and "
                  "binding to it");
            }
            Lval* lval_sym =
                list_nth(node->data.head, 2); /* sym to bind msg to */

            if (lval_sym->type != SYMBOL) {
              release(lval_sym);
              release(ret);
              return make_lval_err(
                  "Expected symbol in catch clause to bind exception message "
                  "to.");
            };

            Lval* lval_str = make_lval_str(ret->data.str);

            Lenv* catch_env = lenv_new();
            catch_env->parent_env = retain(env);
            lenv_put(catch_env, lval_sym, lval_str);
            release(lval_sym);
            release(lval_str);
            Lval* catch_body = make_lval_list();  // 86
            catch_body->data.head = retain(node->data.head->cdr->cdr->cdr);

            release(ret);
            ret = do_list(catch_env, catch_body, RETURN_ON_ERROR);
            release(catch_env);
            release(catch_body);

            if (ret->group == LVAL_ERR) {
              return ret;
            }
          }
        }
        /* FINALLY node =================== */
        else if (is_fn_call(node, "finally", 1)) {
          ddebug("from EXPR, found finally\n");
          mode = FINALLY;
          Lval* body = make_lval_list();
          body->data.head = list_rest(node->data.head);
          Lval* finally_ret = do_list(env, body, RETURN_ON_ERROR);
          release(body);
          if (finally_ret->group == LVAL_ERR) {
            release(ret);
            return finally_ret;
          }

          release(finally_ret);
          arg = iter_next(i);
          continue;
        }
        /* Normal node, but skip if we already have a value that's an error */
        else if (ret->group != LVAL_ERR) {
          release(ret);
          ret = lval_eval(env, node);
          /* printf("evalling expr node\n"); */
          /* lval_println(ret); */
          if (ret->group == LVAL_ERR) {
            if (ret->type == SYS) {
              // We're not catching system errors
              return ret;
            }
          }
        }
        break;
      case CATCH:
        ddebug("in CATCH\n");
        if (is_fn_call(node, "catch", 1)) {
          ddebug("Catch clauses after the first one are ignored for now.\n");
        } else if (is_fn_call(node, "finally", 1)) {
          ddebug("from CATCH, found finally\n");
          mode = FINALLY;
          scoped Lval* body = make_lval_list();
          body->data.head = list_rest(node->data.head);
          Lval* finally_ret = do_list(env, body, RETURN_ON_ERROR);

          if (finally_ret->group == LVAL_ERR) {
            release(ret);
            return finally_ret;
          }
          release(finally_ret);
        } else {
          release(ret);
          return make_lval_err(
              "Only catch or finally clause can follow catch in try "
              "expression");
        }
        break;
      case FINALLY:
        /* printf("in FINALLy\n"); */
        release(ret);
        return make_lval_err("No clause can follow finally in try expression");
    }
    arg = iter_next(i);
  }

  return ret;
}

Lval* eval_do(Lenv* env, Lval* body) {
  return do_list(env, body, RETURN_ON_ERROR);
}

Lval* eval_let(Lenv* env, Lval* arg_list) {
  ddebug("ENV IN LET:");
  /* lenv_print(env); */
  LASSERT(arg_list, list_count(arg_list->data.head) >= 1,
          "Error: let needs at least binding vector")
  scoped_iter Cell* a = iter_new(arg_list);
  Lval* bindings = iter_next(a);
  LASSERT_TYPE("let", arg_list, 0, LVAL_COLLECTION, VECTOR, bindings);
  LASSERT(arg_list, list_count(bindings->data.head) % 2 == 0,
          "Binding vector for let has odd number of forms");

  Lenv* let_env = lenv_new();
  let_env->parent_env = retain(env);

  scoped_iter Cell* b = iter_new(bindings);
  Lval* lval_sym = iter_next(b);

  while (lval_sym) {
    if (lval_sym->type != SYMBOL) {
      release(let_env);
      return make_lval_err("Canot bind non-symbol. Got %s, expected %s.",
                           lval_type_to_name(lval_sym),
                           lval_type_constant_to_name(SYMBOL));
    }

    Lval* lval = iter_next(b);
    lval = lval_eval(let_env, lval);
    if (lval->group == LVAL_ERR) {
      release(let_env);
      return lval;
    }
    let_env = lenv_prepend(let_env, retain(lval_sym), lval);
    lval_sym = iter_next(b);
  }

  scoped Lval* body = make_lval_list();
  body->data.head = retain(iter_cell(a));
  Lval* ret = do_list(let_env, body, RETURN_ON_ERROR);
  release(let_env);
  return ret;
}

/* Not really a special form */
Lval* eval_throw(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("throw", 1);
  ITER_NEXT_TYPE(LVAL_LITERAL, STRING)
  Lval* lval_str = arg;
  ITER_END
  Lval* lval_exc = make_lval_exception(lval_str->data.str);
  return lval_exc;
}

/* https://clojure.org/reference/special_forms */
CFn special_builtins[] = {

    {"quote", eval_quote},
    {"quasiquote", eval_quasiquote},
    {"def", eval_def},
    {"if", eval_if}, /* TCO! */
    {"fn*", eval_lambda},
    {"macro", eval_macro},
    {"try", eval_try},
    {"throw", eval_throw},
    {"do", eval_do},   /* TCO! */
    {"let", eval_let}, /* TCO! */
    {NIL}
    /* Not really needed because we have tco, but for clojure compatibility */
    /* {"loop", eval_loop}, */

};
