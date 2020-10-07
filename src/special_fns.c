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

int number_len(int n) {
  int len = 1;
  while (1) {
    n = n / 10;
    if (n)
      len++;
    else
      return len;
  }
}

char* make_cname(char* name) {
  int uid = get_current_ns()->uid_counter++;
  int uid_length = number_len(uid);
  char* cname = lalloc_size(_strlen(name) + uid_length + 1);
  sprintf(cname, "%s_%d", name, uid);
  return cname;
}

Lval* eval_def(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("def", 2);
  ITER_NEXT_TYPE(LVAL_SYMBOL, -1);
  Lval* lval_sym = arg;
  /* printf("\n++++++++++++in eval_def\n"); */
  /* lval_println(lval_sym); */
  ITER_NEXT;
  Lval* lval = arg;
  ITER_END;
  lval = lval_eval(env, lval);
  if (lval->type == LVAL_ERR) return lval;
  Namespace* ns = get_current_ns();
  if (lenv_is_bound(ns->env, lval_sym)) {
    warn("WARNING: %s already refers to: #'%s/%s in namespace: %s\n",
         lval_sym->str, ns->namespace, lval_sym->str, ns->namespace);
  } else {
    if (lenv_is_bound(get_root_env(env), lval_sym)) {
      warn(
          "WARNING: %s already refers to: #'%s/%s in namespace: %s, "
          "being replaced by: #'%s/%s\n",
          lval_sym->str, config->stdlib, lval_sym->str, config->stdlib,
          ns->namespace, lval_sym->str);
    }
  }

  if (!lval->cname) {
    // We need to keep track of in which namespace a fn is actually defined and
    // under which name so we can refer to its fn_table_index from other
    // namespaces in compiled code by imported global.

    /* printf("def of lambda: %s/%s\n", get_current_ns()->namespace, */
    /*        lval_sym->str); */
    /* lval_println(lval); */

    // Set a unique cname on every lval we add to our environment
    lval->cname = make_cname(lval_sym->str);
    lval->ns = retain(get_current_ns());
  }
  lenv_put(ns->env, lval_sym, lval);
  release(lval);
  return make_lval_nil();
}

Lval* eval_if(Lenv* env, Lval* arg_list) {
  ITER_NEW_MIN_MAX("if", 2, 3);
  ITER_NEXT;
  scoped Lval* cond = lval_eval(env, arg);
  if (cond->type == LVAL_ERR) return retain(cond);
  Lval* branch = NIL;
  if (cond->subtype != LFALSE && cond->subtype != LNIL) { /* TRUE */
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

Lval* eval_lambda_form(Lenv* env, Lval* arg_list, int subtype) {
  ITER_NEW_MIN("fn", 1)
  ITER_NEXT_TYPE(LVAL_COLLECTION, VECTOR);
  Lval* lval_params = arg;

  Cell* param = lval_params->head;

  // Includes any rest arg, so [a & b] has param_count = 2
  int param_count = 0;
  int rest_arg_index = 0;
  int offset = 0;
  while (param) {
    LASSERT(arg_list, ((Lval*)param->car)->type == LVAL_SYMBOL,
            "Canot bind non-symbol. Got %s, expected %s.",
            lval_type_to_name(param->car),
            lval_type_constant_to_name(LVAL_SYMBOL));
    if (rest_arg_index && param_count == rest_arg_index) {
      lval_println(lval_params);
      return make_lval_err(
          "Function format invalid. "
          "Symbol '&' not followed by single symbol.");
      /* return make_lval_err("ERROR: only one rest arg allowed"); */
    }
    Lval* lval_sym = param->car;
    if (_strcmp(lval_sym->str, "&") == 0) {
      rest_arg_index = offset + 1;  // number of params
      param = param->cdr;
      continue;
    }
    offset++;
    /* Lval* lval_ref = make_lval_ref(context, PARAM, param_count + 1); */
    /* lenv_put(params_env, lval_sym, lval_ref); */
    /* release(lval_ref); */

    param_count++;
    param = param->cdr;
  }

  if (rest_arg_index && param_count != rest_arg_index)
    return make_lval_err(
        "Function format invalid. "
        "Symbol '&' not followed by single symbol.");

  if (param_count > MAX_FN_PARAMS)
    return make_lval_err("A function cannot have more than 20 parameters");
  /* return make_lval_err("ERROR: rest arg missing"); */

  // Creates lambda lval and sets the parent_env of its env field (bindings)
  // to the passed in env
  Lval* lval_body = make_lval_list();
  lval_body->head = list_rest(arg_list->head);
  Lenv* closure_env = lenv_new();
  /* closure_env->parent_env = retain(env); */
  closure_env->parent_env = retain(env->parent_env);
  closure_env->kv = retain(env->kv);
  ddebug("lambda has retained env: %d ", is_ns_env(env));
  /* lenv_print(env); */
  ddebug("refcount: %d\n", get_ref_count(env));
  Lval* fn =
      make_lval_lambda(closure_env, retain(lval_params), lval_body, subtype);
  fn->param_count = param_count;
  fn->rest_arg_index = rest_arg_index;

  return fn;
}

Lval* eval_lambda(Lenv* env, Lval* arg_list) {
  return eval_lambda_form(env, arg_list, LAMBDA);
}

// Macros close over the environment where they are defined.
Lval* eval_macro(Lenv* env, Lval* arg_list) {
  return eval_lambda_form(env, arg_list, MACRO);
}

int is_fn_call(Lval* lval, char* sym, int min_node_count) {
  if (lval->type == LVAL_COLLECTION && lval->subtype == LIST &&
      list_count(lval->head) >= min_node_count) {
    lval = lval->head->car;
    return lval->type == LVAL_SYMBOL && _strcmp(lval->str, sym) == 0;
  }
  return 0;
}

Lval* eval_unquote(Lenv* env, Lval* lval) {
  LASSERT_LIST_COUNT(lval, lval->head->cdr, 1, "unquote");
  return lval_eval(env, lval->head->cdr->car);
}

Lval* eval_splice_unquote(Lenv* env, Lval* lval) {
  LASSERT_LIST_COUNT(lval, lval->head->cdr, 1, "splice-unquote");
  Lval* ret = lval_eval(env, lval->head->cdr->car);
  if (ret->type == LVAL_ERR) return ret;
  if (ret->subtype == LNIL) return ret;
  LASSERT_LVAL_IS_LIST_TYPE(ret, "splice-unquote");
  return ret;
}

Lval* eval_quasiquote_nodes(Lenv* env, Lval* arg_list) {
  scoped_iter Cell* i = iter_new(arg_list);
  Lval* arg = iter_next(i);

  /* a place to store processed cells */
  Lval* evalled_list = make_lval_list();
  Cell** lp;
  lp = &evalled_list->head;
  Cell* new_cell;
  while (arg) {
    /* Take the first of the remaining nodes*/
    if (is_fn_call(arg, "unquote", 2)) {
      /* Eval if node is unquoted */
      Lval* unquoted_lval = eval_unquote(env, arg);
      if (unquoted_lval->type == LVAL_ERR) {
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
      if (splice_unquoted_eval->type == LVAL_ERR) {
        release(evalled_list);
        return splice_unquoted_eval;
      }
      /* And concat to processed nodes */
      *lp = retain(splice_unquoted_eval->head);
      release(splice_unquoted_eval);
      Cell* last_cell = list_last(splice_unquoted_eval->head);
      release(last_cell);  //???????
      lp = &(last_cell->cdr);
    } else {
      /* if node is a list apply quasiquote recursively */
      if (arg->type == LVAL_COLLECTION) {
        int subtype = arg->subtype;
        arg = eval_quasiquote_nodes(env, arg);
        if (arg->type == LVAL_ERR) {
          release(evalled_list);
          return arg;
        }
        arg->subtype = subtype;
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
  switch (qq_arg->type) {
    case LVAL_COLLECTION:
      switch (qq_arg->subtype) {
        case LIST:
          if (is_fn_call(qq_arg, "unquote", 2)) {
            ret = eval_unquote(env, qq_arg);
            break;
          }
          LASSERT(arg_list, !is_fn_call(qq_arg, "splice-unquote", 2),
                  "Trying to splice unquote a %s. Can only use splice "
                  "unquote in a "
                  "quasiquoted list",
                  lval_type_to_name(qq_arg->head->cdr->car));
          ret = eval_quasiquote_nodes(env, qq_arg);
          break;
        case VECTOR:
          ret = eval_quasiquote_nodes(env, qq_arg);
          ret->subtype = VECTOR;
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
          if (ret->type == LVAL_ERR) {
            if (list_count(node->head) < 4) {
              release(ret);
              return make_lval_err(
                  "catch clause should have at least an exception type and "
                  "binding to it");
            }
            Lval* lval_sym = list_nth(node->head, 2); /* sym to bind msg to */

            if (lval_sym->type != LVAL_SYMBOL) {
              release(lval_sym);
              release(ret);
              return make_lval_err(
                  "Expected symbol in catch clause to bind exception message "
                  "to.");
            };

            Lval* lval_str = make_lval_str(ret->str);

            Lenv* catch_env = lenv_new();
            catch_env->parent_env = retain(env);
            lenv_put(catch_env, lval_sym, lval_str);
            release(lval_sym);
            release(lval_str);
            Lval* catch_body = make_lval_list();  // 86
            catch_body->head = retain(node->head->cdr->cdr->cdr);

            release(ret);
            ret = do_list(catch_env, catch_body, RETURN_ON_ERROR);
            release(catch_env);
            release(catch_body);

            if (ret->type == LVAL_ERR) {
              return ret;
            }
          }
        }
        /* FINALLY node =================== */
        else if (is_fn_call(node, "finally", 1)) {
          ddebug("from EXPR, found finally\n");
          mode = FINALLY;
          Lval* body = make_lval_list();
          body->head = list_rest(node->head);
          Lval* finally_ret = do_list(env, body, RETURN_ON_ERROR);
          release(body);
          if (finally_ret->type == LVAL_ERR) {
            release(ret);
            return finally_ret;
          }

          release(finally_ret);
          arg = iter_next(i);
          continue;
        }
        /* Normal node, but skip if we already have a value that's an error */
        else if (ret->type != LVAL_ERR) {
          release(ret);
          ret = lval_eval(env, node);
          /* printf("evalling expr node\n"); */
          /* lval_println(ret); */
          if (ret->type == LVAL_ERR) {
            if (ret->subtype == SYS) {
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
          body->head = list_rest(node->head);
          Lval* finally_ret = do_list(env, body, RETURN_ON_ERROR);

          if (finally_ret->type == LVAL_ERR) {
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
  LASSERT(arg_list, list_count(arg_list->head) >= 1,
          "Error: let needs at least binding vector")
  scoped_iter Cell* a = iter_new(arg_list);
  Lval* bindings = iter_next(a);
  LASSERT_TYPE("let", arg_list, 0, LVAL_COLLECTION, VECTOR, bindings);
  LASSERT(arg_list, list_count(bindings->head) % 2 == 0,
          "Binding vector for let has odd number of forms");

  Lenv* let_env = lenv_new();
  let_env->parent_env = retain(env);

  scoped_iter Cell* b = iter_new(bindings);
  Lval* lval_sym = iter_next(b);

  while (lval_sym) {
    if (lval_sym->type != LVAL_SYMBOL) {
      release(let_env);
      return make_lval_err("Canot bind non-symbol. Got %s, expected %s.",
                           lval_type_to_name(lval_sym),
                           lval_type_constant_to_name(LVAL_SYMBOL));
    }

    Lval* lval = iter_next(b);
    lval = lval_eval(let_env, lval);
    if (lval->type == LVAL_ERR) {
      release(let_env);
      return lval;
    }
    Lenv* new_let_env = lenv_prepend(let_env, lval_sym, lval);
    release(lval);
    new_let_env->parent_env = let_env;

    let_env = new_let_env;
    lval_sym = iter_next(b);
  }

  scoped Lval* body = make_lval_list();
  body->head = retain(iter_cell(a));
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
  Lval* lval_exc = make_lval_exception(lval_str->str);
  return lval_exc;
}

/* https://clojure.org/reference/special_forms */
RuntimeFn special_builtins[] = {

    {"quote", eval_quote},
    {"quasiquote", eval_quasiquote},
    {"def", eval_def},
    {"if", eval_if}, /* TCO! */
    {"fn", eval_lambda},
    {"macro", eval_macro},
    {"try", eval_try},
    {"throw", eval_throw},
    {"do", eval_do},   /* TCO! */
    {"let", eval_let}, /* TCO! */
    {NIL}
    /* Not really needed because we have tco, but for clojure compatibility */
    /* {"loop", eval_loop}, */

};
