#include "special_fns.h"

#include "assert.h"
#include "env.h"
#include "eval.h"
#include "io.h"
#include "lib.h"
#include "list_fns.h"
#include "lval.h"
#include "print.h"

Lval* eval_quote(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "quote");
  return lval_take(sexpr_args, 0);
}

Lval* eval_def(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 2, "def");
  LASSERT_NODE_TYPE(sexpr_args, 0, LVAL_SYMBOL, "def");
  Lval* lval_sym = sexpr_args->node[0];
  if (lenv_is_bound(get_root_env(env), lval_sym)) {
    printf(
        "WARNING: %s already refers to: #'root-env/%s in namespace: user, "
        "being replaced by: #'user/%s",
        lval_sym->sym, lval_sym->sym, lval_sym->sym);
  }

  Lval* lval = lval_pop(sexpr_args, 1);

  /* lval_println(lval); */
  lval = lval_eval(env, lval);

  /* lval_println(lval); */
  if (lval->type == LVAL_ERR) {
    lval_del(sexpr_args);
    return lval;
  }
  lenv_put(env, lval_sym, lval);
  lval_del(sexpr_args);
  return make_lval_list();
}

Lval* eval_if(Lenv* env, Lval* sexpr_args) {
  LASSERT(sexpr_args, sexpr_args->count == 2 || sexpr_args->count == 3,
          "Expecting either 2 or 3 arguments to if, got %d", sexpr_args->count);
  /* COND */
  Lval* cond = lval_pop(sexpr_args, 0);
  cond = lval_eval(env, cond);
  if (cond->type == LVAL_ERR) {
    lval_del(sexpr_args);
    return cond;
  }
  if (cond->subtype != NUMBER) {
    Lval* lval_err =
        make_lval_err("if passed incorrect type for cond, got %s, expected %s",
                      lval_type_to_name2(cond), lval_type_to_name(NUMBER));
    lval_del(cond);
    lval_del(sexpr_args);
    return lval_err;
  }
  Lval* ret = NULL;
  if (cond->num) {
    /* TRUE */
    Lval* body_true = lval_pop(sexpr_args, 0);
    body_true->tco_env = env;
    ret = body_true;
  } else {
    /* FALSE  */
    if (sexpr_args->count == 2) {
      Lval* body_false = lval_pop(sexpr_args, 1);
      body_false->tco_env = env;
      ret = body_false;
    }
  }
  lval_del(sexpr_args);
  return ret ? ret : make_lval_list();
  /* return ret ? lval_eval(env, ret) : make_lval_sexpr(); */
}

Lval* eval_lambda_form(Lenv* env, Lval* sexpr_args, int subtype) {
  LASSERT(sexpr_args, sexpr_args->count >= 1,
          "Error: function needs at least a parameter vector")
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, VECTOR, "fn");

  for (int i = 0; i < sexpr_args->node[0]->count; ++i) {
    LASSERT(sexpr_args, sexpr_args->node[0]->node[i]->type == LVAL_SYMBOL,
            "Canot bind non-symbol. Got %s, expected %s.",
            lval_type_to_name2(sexpr_args->node[0]->node[i]),
            lval_type_to_name(LVAL_SYMBOL));
  }

  Lval* formals = lval_pop(sexpr_args, 0);

  // Creates lambda lval and sets the parent_env of its env field (bindings) to
  // the passed in env
  return make_lval_lambda(env, formals, sexpr_args, subtype);
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
  return lval->type == LVAL_COLLECTION && lval->subtype == LIST &&
         lval->count >= min_node_count && lval->node[0]->type == LVAL_SYMBOL &&
         _strcmp(lval->node[0]->sym, sym) == 0;
}

Lval* eval_unquote(Lenv* env, Lval* lval) {
  LASSERT_NODE_COUNT(lval, 2, "unquote");
  return lval_eval(env, lval_take(lval, 1));
}

Lval* eval_splice_unquote(Lenv* env, Lval* lval) {
  LASSERT_NODE_COUNT(lval, 2, "splice-unquote");
  Lval* ret = lval_eval(env, lval_take(lval, 1));
  if (ret->type == LVAL_ERR) return ret;
  LASSERT_LVAL_IS_LIST_TYPE(ret, "splice-unquote");
  return ret;
}

Lval* eval_quasiquote_nodes(Lenv* env, Lval* lval) {
  /* a place to store processed nodes */
  Lval* processed_nodes = make_lval_list();
  while (lval->count) {
    /* Take the first of the remaining nodes*/
    Lval* node = lval_pop(lval, 0);
    if (is_fn_call(node, "unquote", 2)) {
      /* Eval if node is unquoted */
      Lval* unquoted_lval = eval_unquote(env, node);
      if (unquoted_lval->type == LVAL_ERR) {
        lval_del(lval);
        lval_del(processed_nodes);
        return unquoted_lval;
      }
      /* And add to processed nodes */
      lval_add_child(processed_nodes, unquoted_lval);

    } else if (is_fn_call(node, "splice-unquote", 2)) {
      /* Eval if node is splice unquoted */
      Lval* splice_unquoted_eval = eval_splice_unquote(env, node);
      if (splice_unquoted_eval->type == LVAL_ERR) {
        lval_del(lval);
        lval_del(processed_nodes);
        return splice_unquoted_eval;
      }
      /* And concat to processed nodes */
      processed_nodes = lval_concat(processed_nodes, splice_unquoted_eval);
    } else {
      /* if node is a list apply quasiquote recursively */
      if (node->type == LVAL_COLLECTION)
        node = eval_quasiquote_nodes(env, node);
      if (node->type == LVAL_ERR) {
        lval_del(lval);
        lval_del(processed_nodes);
        return node;
      }
      /* And add to processed nodes */
      lval_add_child(processed_nodes, node);
    }
  }
  /* Point lval->node to processed nodes */
  lval->count = processed_nodes->count;
  lval->node = processed_nodes->node;
  /* And clean up processed nodes */
  processed_nodes->count = 0;
  /* free(processed_nodes); */
  return lval;
}

/* Symbols/Lists/Vectors/Sets/Maps */
Lval* eval_quasiquote(Lenv* env, Lval* sexpr_args) {
  /* printf("quasiquote-----\n"); */
  LASSERT_NODE_COUNT(sexpr_args, 1, "quasiquote");
  Lval* arg = lval_take(sexpr_args, 0);
  /* printf("arg:\n"); */
  /* lval_println(arg); */
  Lval* ret = NULL;
  switch (arg->type) {
    case LVAL_COLLECTION:
      switch (arg->subtype) {
        case LIST:
          if (is_fn_call(arg, "unquote", 2)) {
            ret = eval_unquote(env, arg);
            break;
          }
          LASSERT(
              sexpr_args, !is_fn_call(arg, "splice-unquote", 2),
              "Trying to splice unquote a %s. Can only use splice unquote in a "
              "quasiquoted list",
              lval_type_to_name2(arg->node[1]));

          ret = eval_quasiquote_nodes(env, arg);
          break;
        case VECTOR:
          ret = eval_quasiquote_nodes(env, arg);
          break;
        case MAP:

          /* todo  */
          ret = arg;
      }
      break;
    default: /* symbols, strings, numbers etc */
      ret = arg;
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
            "Only catch or finally clause can follow catch in try expression");
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
Lval* eval_throw(Lenv* env, Lval* sexpr_args) {
  LASSERT(sexpr_args, sexpr_args->count >= 1, "Error: throw needs a message");
  sexpr_args = eval_nodes(env, sexpr_args, sexpr_args->count);
  if (sexpr_args->type == LVAL_ERR) {
    lval_del(sexpr_args);
    return sexpr_args;
  }
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, STRING, "throw");
  char* msg = sexpr_args->node[0]->str;
  Lval* lval_exc = make_lval_exception(msg);
  lval_del(sexpr_args);
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
