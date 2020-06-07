#include "special_fns.h"

#include "assert.h"
#include "env.h"
#include "eval.h"
#include "io.h"
#include "iter.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list_fns.h"
#include "lval.h"
#include "print.h"

Lval* eval_quote(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("quote", 1);
  ITER_NEXT;
  Lval* ret = arg;
  ITER_END;
  return ret;
}

Lval* eval_def(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("def", 2);
  ITER_NEXT_TYPE(LVAL_SYMBOL, NIL);
  Lval* lval_sym = arg;
  if (lenv_is_bound(get_root_env(env), lval_sym)) {
    printf(
        "WARNING: %s already refers to: #'root-env/%s in namespace: user, "
        "being replaced by: #'user/%s",
        lval_sym->sym, lval_sym->sym, lval_sym->sym);
  }
  ITER_NEXT;
  Lval* lval = arg;
  ITER_END;
  lval = lval_eval(env, lval);
  if (lval->type == LVAL_ERR) return lval;
  lenv_put(env, lval_sym, lval);
  lval_del(arg_list);
  return make_lval_list();
}

Lval* eval_if(Lenv* env, Lval* arg_list) {
  ITER_NEW_MIN_MAX("if", 2, 3);
  ITER_NEXT_TYPE(LVAL_LITERAL, NUMBER);
  Lval* cond = lval_eval(env, arg);
  if (cond->type == LVAL_ERR) return cond;
  Lval* ret = NULL;
  if (cond->num) { /* TRUE */
    ITER_NEXT;
    ret = arg;
    ITER_NEXT;
  } else { /* FALSE  */
    ITER_NEXT;
    ITER_NEXT;
    ret = arg;
  }
  ITER_END;
  if (ret) {
    ret->tco_env = env;
    return ret;
  }
  return make_lval_list();
}

Lval* eval_lambda_form(Lenv* env, Lval* arg_list, int subtype) {
  ITER_NEW_MIN("fn", 1)
  ITER_NEXT_TYPE(LVAL_COLLECTION, VECTOR);
  Lval* lval_params = arg;

  Cell* param = arg->list;
  while (param) {
    LASSERT(arg_list, ((Lval*)param->car)->type == LVAL_SYMBOL,
            "Canot bind non-symbol. Got %s, expected %s.",
            lval_type_to_name2(param->car), lval_type_to_name(LVAL_SYMBOL));
    param = param->cdr;
  }

  // Creates lambda lval and sets the parent_env of its env field (bindings)
  // to the passed in env
  Lval* lval_body = make_lval_list();
  lval_body->list = list_rest(arg_list->list);
  Lval* fn = make_lval_lambda(env, lval_params, lval_body, subtype);
  return fn;
}

Lval* eval_lambda(Lenv* env, Lval* sexpr_args) {
  return eval_lambda_form(env, sexpr_args, LAMBDA);
}

// When macros are defined they close over the environment where they are
// defined.
Lval* eval_macro(Lenv* env, Lval* sexpr_args) {
  return eval_lambda_form(env, sexpr_args, MACRO);
}

int is_fn_call(Lval* lval, char* sym, int min_node_count) {
  if (lval->type == LVAL_COLLECTION && lval->subtype == LIST &&
      list_count(lval->list) >= min_node_count) {
    lval = lval->list->car;
    return lval->type == LVAL_SYMBOL && _strcmp(lval->sym, sym) == 0;
  }
  return 0;
}

Lval* eval_unquote(Lenv* env, Lval* lval) {
  LASSERT_LIST_COUNT(lval, lval->list->cdr, 1, "unquote");
  return lval_eval(env, lval->list->cdr->car);
}

Lval* eval_splice_unquote(Lenv* env, Lval* lval) {
  LASSERT_LIST_COUNT(lval, lval->list->cdr, 1, "splice-unquote");
  Lval* ret = lval_eval(env, lval->list->cdr->car);
  if (ret->type == LVAL_ERR) return ret;
  LASSERT_LVAL_IS_LIST_TYPE(ret, "splice-unquote");
  return ret;
}

Lval* eval_quasiquote_nodes(Lenv* env, Lval* arg_list) {
  /* a place to store processed cells */
  Lval* evalled_list = make_lval_list();
  ITER_NEW("quasiquote")
  ITER_NEXT
  Cell** lp;
  lp = &evalled_list->list;
  Cell* new_cell;
  while (arg) {
    /* Take the first of the remaining nodes*/
    if (is_fn_call(arg, "unquote", 2)) {
      /* Eval if node is unquoted */
      Lval* unquoted_lval = eval_unquote(env, arg);
      if (unquoted_lval->type == LVAL_ERR) {
        lval_del(arg);
        lval_del(evalled_list);
        ITER_END
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
        lval_del(arg_list);
        lval_del(evalled_list);
        ITER_END
        return splice_unquoted_eval;
      }
      /* And concat to processed nodes */
      *lp = splice_unquoted_eval->list;
      Cell* last_cell = list_last(splice_unquoted_eval->list);
      lp = &(last_cell->cdr);
    } else {
      /* if node is a list apply quasiquote recursively */

      if (arg->type == LVAL_COLLECTION) arg = eval_quasiquote_nodes(env, arg);
      if (arg->type == LVAL_ERR) {
        lval_del(arg_list);
        lval_del(evalled_list);
        ITER_END
        return arg;
      }
      new_cell = make_cell();
      new_cell->car = arg;
      *lp = new_cell;
      lp = &(new_cell->cdr);
    }
    ITER_NEXT
  }
  ITER_END
  return evalled_list;
}

/* Symbols/Lists/Vectors/Sets/Maps */
Lval* eval_quasiquote(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("quasiquote", 1)
  ITER_NEXT
  Lval* qq_arg = arg;
  ITER_END;
  Lval* ret = NULL;
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
                  lval_type_to_name2(qq_arg->list->cdr->car));
          ret = eval_quasiquote_nodes(env, qq_arg);
          break;
        case VECTOR:
          ret = eval_quasiquote_nodes(env, qq_arg);
          break;
        case MAP:

          /* todo  */
          ret = qq_arg;
      }
      break;
    default: /* symbols, strings, numbers etc */
      ret = qq_arg;
  }
  return ret;
}

/* Implemented as a little state machine */
enum { EXPR, CATCH, FINALLY };
Lval* eval_try(Lenv* env, Lval* sexpr_args) {
  int mode = EXPR;
  Lval* ret = make_lval_list();
  Lval* node;

  while (sexpr_args->count) {
    /* for (int i = 0; i < sexpr_args->count; ++i) { */
    node = lval_pop(sexpr_args, 0);
    /* node = sexpr_args->node[i]; */
    /* printf("node: "); */
    /* lval_println(node); */
    switch (mode) {
      case EXPR:
        /* printf("in EXPR\n"); */
        /* CATCH node =================== */
        if (is_fn_call(node, "catch", 1)) {
          /* printf("from EXPR, found catch\n"); */
          mode = CATCH;
          if (ret->type == LVAL_ERR) {
            lval_del(lval_pop(node, 0)); /* catch */
            if (node->count < 2) {
              lval_del(node);
              lval_del(ret);
              lval_del(sexpr_args);
              return make_lval_err(
                  "catch clause should have at least an exception type and "
                  "binding to it");
            }
            Lenv* catch_env = lenv_new();
            catch_env->parent_env = env;
            lval_del(lval_pop(node, 0));        /* ignored ( eg Exception) */
            Lval* lval_sym = lval_pop(node, 0); /* sym to bind msg to */
            if (lval_sym->type != LVAL_SYMBOL) {
              lval_del(lval_sym);
              lval_del(node);
              lval_del(ret);
              lval_del(sexpr_args);
              return make_lval_err(
                  "Expected symbol in catch clause to bind exception message "
                  "to.");
            };

            lenv_put(catch_env, lval_sym, make_lval_str(ret->err));
            lval_del(lval_sym);
            lval_del(ret);
            /* lenv_print(catch_env); */
            ret = eval_body(catch_env, node, WITHOUT_TCO);
            lenv_del(catch_env);
            if (ret->type == LVAL_ERR) {
              lval_del(sexpr_args);
              return ret;
            }
          }
          continue;
        }
        /* FINALLY node =================== */
        if (is_fn_call(node, "finally", 1)) {
          mode = FINALLY;
          lval_del(lval_pop(node, 0)); /* finally */
          Lval* finally_ret = eval_body(env, node, WITHOUT_TCO);
          if (finally_ret->type == LVAL_ERR) {
            lval_del(ret);
            lval_del(sexpr_args);
            return finally_ret;
          }
          lval_del(finally_ret);
          continue;
        }
        /* Normal node, but skip if we already have a value that's an error */
        if (ret->type != LVAL_ERR) {
          lval_del(ret);
          ret = lval_eval(env, node);
          /* printf("evalling expr node\n"); */
          /* lval_println(ret); */
          if (ret->type == LVAL_ERR) {
            if (ret->subtype == SYS) {
              lval_del(sexpr_args);
              return ret;
            }
          }
        } else {
          lval_del(node);
        }

        break;
      case CATCH:
        /* printf("in CATCH\n"); */
        if (is_fn_call(node, "catch", 1)) {
          printf("Catch clauses after the first one are ignored for now.\n");
          lval_del(node);
          continue;
        }
        if (is_fn_call(node, "finally", 1)) {
          mode = FINALLY;
          lval_del(lval_pop(node, 0)); /* finally */
          Lval* finally_ret = eval_body(env, node, WITHOUT_TCO);
          if (finally_ret->type == LVAL_ERR) {
            lval_del(ret);
            lval_del(sexpr_args);
            return finally_ret;
          }
          lval_del(finally_ret);
          continue;
        }
        lval_del(sexpr_args);
        return make_lval_err(
            "Only catch or finally clause can follow catch in try "
            "expression");
      case FINALLY:
        /* printf("in FINALLy\n"); */
        lval_del(sexpr_args);
        lval_del(node);
        lval_del(ret);
        return make_lval_err("No clause can follow finally in try expression");
    }
  }
  lval_del(sexpr_args);
  return ret;
}

Lval* eval_do(Lenv* env, Lval* body) {
  return eval_body(env, body, WITHOUT_TCO);
}

Lval* eval_let(Lenv* env, Lval* sexpr_args) {
  LASSERT(sexpr_args, sexpr_args->count >= 1,
          "Error: let needs at least binding vector")
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, VECTOR, "fn");
  Lval* bindings = lval_pop(sexpr_args, 0);

  LASSERT(sexpr_args, bindings->count % 2 == 0,
          "Binding vector for let has odd number of forms");

  Lenv* let_env = lenv_new();
  let_env->parent_env = env;

  Lval* lval_sym;
  while (bindings->count) {
    lval_sym = lval_pop(bindings, 0);
    LASSERT(sexpr_args, lval_sym->type == LVAL_SYMBOL,
            "Canot bind non-symbol. Got %s, expected %s.",
            lval_type_to_name2(lval_sym), lval_type_to_name(LVAL_SYMBOL));

    Lval* lval = lval_eval(env, lval_pop(bindings, 0));
    if (lval->type == LVAL_ERR) {
      lval_del(lval_sym);
      lval_del(bindings);
      lval_del(sexpr_args);
      lenv_del(let_env);
      return lval;
    }
    lenv_put(let_env, lval_sym, lval);
    lval_del(lval_sym);
    lval_del(lval);
  }
  return eval_body(let_env, sexpr_args, WITH_TCO);
}

/* Not really a special form */
Lval* eval_throw(Lenv* env, Lval* arg_list) {
  LASSERT(arg_list, arg_list->count >= 1, "Error: throw needs a message");
  arg_list = eval_nodes(env, arg_list);
  if (arg_list->type == LVAL_ERR) {
    lval_del(arg_list);
    return arg_list;
  }
  LASSERT_NODE_SUBTYPE(arg_list, 0, STRING, "throw");
  char* msg = arg_list->node[0]->str;
  Lval* lval_exc = make_lval_exception(msg);
  lval_del(arg_list);
  return lval_exc;
}

/* https://clojure.org/reference/special_forms */
void lenv_add_special_fns(Lenv* env) {
  lenv_add_builtin(env, "quote", eval_quote, SPECIAL);
  lenv_add_builtin(env, "quasiquote", eval_quasiquote, SPECIAL);
  lenv_add_builtin(env, "def", eval_def, SPECIAL);
  lenv_add_builtin(env, "if", eval_if, SPECIAL); /* no TCO! */
  lenv_add_builtin(env, "fn", eval_lambda, SPECIAL);
  lenv_add_builtin(env, "macro", eval_macro, SPECIAL);
  lenv_add_builtin(env, "try", eval_try, SPECIAL);
  lenv_add_builtin(env, "throw", eval_throw, SPECIAL);
  lenv_add_builtin(env, "do", eval_do, SPECIAL);   /* no TCO! */
  lenv_add_builtin(env, "let", eval_let, SPECIAL); /* no TCO! */

  /* Not really needed because we have tco, but for clojure compatibility */
  /* lenv_add_builtin(env, "loop", eval_loop, SPECIAL); */
}
