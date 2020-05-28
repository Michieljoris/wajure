#include "special_fns.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "env.h"
#include "eval.h"
#include "list_fns.h"
#include "lval.h"
#include "print.h"

Lval* eval_quote(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "quote");
  return lval_take(sexpr_args, 0);
}

Lval* eval_def(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 2, "def");
  LASSERT_NODE_TYPE(sexpr_args, 0, LVAL_SYM, "def");
  Lval* lval_sym = sexpr_args->node[0];
  if (lenv_is_bound(get_root_env(env), lval_sym)) {
    /* printf("Warning: var '%s' is already bound in the root env",
     * lval_sym->sym); */
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
  return make_lval_sexpr();
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
  if (cond->type != LVAL_NUM) {
    Lval* lval_err =
        make_lval_err("if passed incorrect type for cond, got %s, expected %s",
                      lval_type_to_name2(cond), lval_type_to_name(LVAL_NUM));
    lval_del(cond);
    lval_del(sexpr_args);
    return lval_err;
  }
  Lval* ret = NULL;
  if (cond->num) {
    /* TRUE */
    Lval* body_true = lval_pop(sexpr_args, 0);
    ret = lval_eval(env, body_true);
  } else {
    /* FALSE  */
    if (sexpr_args->count == 2) {
      Lval* body_false = lval_pop(sexpr_args, 1);
      ret = lval_eval(env, body_false);
    }
  }
  lval_del(sexpr_args);
  if (ret) {
    return ret;
  }
  return make_lval_sexpr(); /*TODO: should be nil */
}

Lval* eval_lambda(Lenv* env, Lval* sexpr_args) {
  /* printf("in eval_lambda\n"); */
  LASSERT(sexpr_args, sexpr_args->count >= 1,
          "Error: lambda needs at least parameter list")
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, VECTOR, "fn");

  for (int i = 0; i < sexpr_args->node[0]->count; ++i) {
    LASSERT(sexpr_args, sexpr_args->node[0]->node[i]->type == LVAL_SYM,
            "Canot bind non-symbol. Got %s, expected %s.",
            lval_type_to_name2(sexpr_args->node[0]->node[i]),
            lval_type_to_name(LVAL_SYM));
  }

  Lval* formals = lval_pop(sexpr_args, 0);

  Lval* ret = make_lval_lambda(formals, sexpr_args);
  return ret;
}

bool is_fn_call(Lval* lval, char* sym, int min_node_count) {
  return lval->type == LVAL_SEQ && lval->subtype == LIST &&
         lval->count >= min_node_count && lval->node[0]->type == LVAL_SYM &&
         strcmp(lval->node[0]->sym, sym) == 0;
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
  Lval* processed_nodes = make_lval_sexpr();
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
      if (node->type == LVAL_SEQ) node = eval_quasiquote_nodes(env, node);
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
  free(processed_nodes);
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
    case LVAL_SEQ:
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

Lval* eval_macro(Lenv* env, Lval* sexpr_args) {
  LASSERT(sexpr_args, sexpr_args->count >= 1,
          "Error: defmacro needs at least a parameter list")
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, VECTOR, "macro");

  for (int i = 0; i < sexpr_args->node[0]->count; ++i) {
    LASSERT(sexpr_args, sexpr_args->node[0]->node[i]->type == LVAL_SYM,
            "Canot bind non-symbol. Got %s, expected %s.",
            lval_type_to_name2(sexpr_args->node[0]->node[i]),
            lval_type_to_name(LVAL_SYM));
  }

  Lval* formals = lval_pop(sexpr_args, 0);
  return make_lval_macro(formals, sexpr_args);
}

enum { EXPR, CATCH, FINALLY };
Lval* eval_try(Lenv* env, Lval* sexpr_args) {
  int mode = EXPR;
  Lval* ret = make_lval_sexpr();
  Lval* node;

  /* while (sexpr_args->count) { */
  for (int i = 0; i < sexpr_args->count; ++i) {
    node = sexpr_args->node[i];
    /* printf("node: "); */
    /* lval_println(node); */
    switch (mode) {
      case EXPR:
        /* printf("in EXPR\n"); */
        if (is_fn_call(node, "catch", 1)) {
          /* printf("from EXPR, found catch\n"); */
          mode = CATCH;
          if (ret->type == LVAL_ERR) {
            Lenv* catch_env = lenv_new();
            catch_env->parent_env = env;
            lval_del(lval_pop(node, 0));        /* case */
            lval_del(lval_pop(node, 0));        /* ignored ( egException) */
            Lval* lval_sym = lval_pop(node, 0); /* sym to bind msg to */
            if (lval_sym->type != LVAL_SYM) {
              lval_del(lval_sym);
              lval_del(sexpr_args);
              return make_lval_err(
                  "Expected symbol in catch clause to bind exception message "
                  "to.");
            };

            lenv_put(catch_env, lval_sym, make_lval_str(ret->err));
            lval_del(lval_sym);
            lval_del(ret);
            ret = eval_nodes(catch_env, node, node->count);
            if (ret->type == LVAL_ERR) {
              lenv_del(catch_env);
              lval_del(sexpr_args);
              return ret;
            }
            if (ret->count > 0) {
              ret = lval_take(ret, ret->count - 1);
            }
          }
          continue;
        }
        if (is_fn_call(node, "finally", 1)) {
          mode = FINALLY;
          lval_del(lval_pop(node, 0));
          Lval* finally_ret = eval_nodes(env, node, node->count);
          if (finally_ret->type == LVAL_ERR) {
            lval_del(ret);
            lval_del(sexpr_args);
            return finally_ret;
          }
          lval_del(finally_ret);
          continue;
        }
        if (ret->type != LVAL_ERR) {
          lval_del(ret);
          ret = lval_eval(env, node);
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
          continue;
        }
        if (is_fn_call(node, "finally", 1)) {
          mode = FINALLY;
          lval_del(lval_pop(node, 0));
          Lval* finally_ret = eval_nodes(env, node, node->count);
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
        lval_del(ret);
        return make_lval_err("No clause can follow finally in try expression");
    }
  }
  lval_del(sexpr_args);
  return ret;
}

Lval* eval_throw(Lenv* env, Lval* sexpr_args) {
  LASSERT(sexpr_args, sexpr_args->count >= 1,
          "Error: throw needs at a message");
  LASSERT_NODE_TYPE(sexpr_args, 0, LVAL_STR, "throw");
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
  lenv_add_builtin(env, "if", eval_if, SPECIAL);
  lenv_add_builtin(env, "fn", eval_lambda, SPECIAL);
  lenv_add_builtin(env, "macro", eval_macro, SPECIAL);
  lenv_add_builtin(env, "try", eval_try, SPECIAL);
  lenv_add_builtin(env, "throw", eval_throw, SPECIAL);
  /* TODO:  */
  /* lenv_add_builtin(env, "do", eval_do, SPECIAL); */
  /* lenv_add_builtin(env, "let", eval_let, SPECIAL); */
  /* lenv_add_builtin(env, "loop", eval_loop, SPECIAL); */
}

/* typedef struct { */
/*   char* key; */
/*   int val; */
/* } t_symstruct; */

/* enum { QUOTE, QUASIQUOTE, DEF, LAMBDA, IF, MACRO, MACROEXPAND }; */
/* static t_symstruct special_syms[] = {{"quote", QUOTE}, */
/*                                      {"quasiquote", QUASIQUOTE}, */
/*                                      {"def", DEF}, */
/*                                      {"fn", LAMBDA}, */
/*                                      {"if", IF}, */
/*                                      {"macro", MACRO}, */
/*                                      {"macroexpand", MACROEXPAND}}; */
/* long special_syms_count = sizeof(special_syms) / sizeof(t_symstruct); */

/* int lookup_special_sym(char* sym) { */
/*   for (int i = 0; i < special_syms_count; i++) { */
/*     if (strcmp(special_syms[i].key, sym) == 0) { */
/*       return special_syms[i].val; */
/*     } */
/*   } */
/*   return -1; */
/* } */

/* Lval* eval_special(Lenv* env, long special_sym, Lval* sexpr_args) { */
/*   switch (special_sym) { */
/*     case QUOTE: */
/*       LASSERT_NODE_COUNT(sexpr_args, 1, "quote"); */
/*       return lval_take(sexpr_args, 0); */
/*     case QUASIQUOTE: */
/*       return eval_quasiquote(env, sexpr_args); */
/*     case DEF: */
/*       return eval_def(env, sexpr_args); */
/*     case IF: */
/*       return eval_if(env, sexpr_args); */
/*     case LAMBDA: */
/*       return eval_lambda(env, sexpr_args); */
/*     case MACROEXPAND: /\* TODO isn't a special form right *\/ */
/*       return eval_macroexpand(env, sexpr_args); */
/*     case MACRO: */
/*       return eval_macro(env, sexpr_args); */
/*     default: */
/*       return make_lval_err("%li : not implemented yet!!\n", special_sym);
 */
/*   } */
/* } */
