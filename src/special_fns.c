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
  if (lval->type == LVAL_ERR) return lval;
  lenv_put(env, lval_sym, lval);
  lval_del(sexpr_args);
  return make_lval_sexpr();
}

Lval* eval_if(Lenv* env, Lval* sexpr_args) {
  LASSERT(sexpr_args, sexpr_args->count == 2 || sexpr_args->count == 3,
          "Expecting either 2 or 3 arguments to if, got %d", sexpr_args->count);
  Lval* cond = lval_pop(sexpr_args, 0);
  cond = lval_eval(env, cond);
  if (cond->type == LVAL_ERR) return cond;
  LASSERT(sexpr_args, cond->type == LVAL_NUM,
          "if passed incorrect type for cond, got %s, expected %s",
          lval_type_to_name2(cond), lval_type_to_name(LVAL_NUM));
  Lval* ret = NULL;
  if (cond->num) {
    Lval* body_true = lval_pop(sexpr_args, 0);
    ret = lval_eval(env, body_true);
  } else {
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

bool is_unquoted(Lval* lval) {
  return lval->type == LVAL_SEQ && lval->count >= 2 &&
         lval->node[0]->type == LVAL_SYM &&
         strcmp(lval->node[0]->sym, "unquote") == 0;
}

bool is_splice_unquoted(Lval* lval) {
  return lval->type == LVAL_SEQ && lval->count >= 2 &&
         lval->node[0]->type == LVAL_SYM &&
         strcmp(lval->node[0]->sym, "splice-unquote") == 0;
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
  Lval* processed_nodes = make_lval_sexpr();
  while (lval->count) {
    Lval* node = lval_pop(lval, 0);
    /* printf("node:\n"); */
    /* lval_println(node); */
    if (is_unquoted(node)) {
      /* printf("unquoted node: "); */
      /* lval_println(node); */
      Lval* unquoted_lval = eval_unquote(env, node);
      /* lval_println(node); */
      if (unquoted_lval->type == LVAL_ERR) return unquoted_lval;
      lval_add_child(processed_nodes, unquoted_lval);
    } else if (is_splice_unquoted(node)) {
      /* printf("splice-unquote\n"); */
      Lval* splice_unquoted_eval = eval_splice_unquote(env, node);
      /* lval_println(splice_unquoted_eval); */

      if (splice_unquoted_eval->type == LVAL_ERR) return splice_unquoted_eval;
      processed_nodes = lval_concat(processed_nodes, splice_unquoted_eval);
    } else {
      if (node->type == LVAL_SEQ) node = eval_quasiquote_nodes(env, node);
      if (node->type == LVAL_ERR) return node;
      lval_add_child(processed_nodes, node);
    }
  }
  lval->count = processed_nodes->count;
  lval->node = processed_nodes->node;
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
          if (is_unquoted(arg)) {
            ret = eval_unquote(env, arg);
            break;
          }
          LASSERT(
              sexpr_args, !is_splice_unquoted(arg),
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

Lval* eval_macroexpand(Lenv* env, Lval* sexpr_args) { return sexpr_args; }

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

/* https://clojure.org/reference/special_forms */
/* TODO: loop recur try throw do */
void lenv_add_special_fns(Lenv* env) {
  lenv_add_builtin(env, "quote", eval_quote, SPECIAL);
  lenv_add_builtin(env, "quasiquote", eval_quasiquote, SPECIAL);
  lenv_add_builtin(env, "def", eval_def, SPECIAL);
  lenv_add_builtin(env, "if", eval_if, SPECIAL);
  lenv_add_builtin(env, "fn", eval_lambda, SPECIAL);
  lenv_add_builtin(env, "macro", eval_macro, SPECIAL);
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
/*       return make_lval_err("%li : not implemented yet!!\n", special_sym); */
/*   } */
/* } */
