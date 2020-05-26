#include "special.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "env.h"
#include "eval.h"
#include "list_fns.h"
#include "lval.h"
#include "print.h"

typedef struct {
  char* key;
  int val;
} t_symstruct;

enum { QUOTE, QUASIQUOTE, DEF, LAMBDA, IF, DEFMACRO };
static t_symstruct special_syms[] = {
    {"quote", QUOTE}, {"quasiquote", QUASIQUOTE}, {"def", DEF}, {"fn", LAMBDA},
    {"if", IF},       {"defmacro", DEFMACRO}};
long special_syms_count = sizeof(special_syms) / sizeof(t_symstruct);

int lookup_special_sym(char* sym) {
  for (int i = 0; i < special_syms_count; i++) {
    if (strcmp(special_syms[i].key, sym) == 0) {
      return special_syms[i].val;
    }
  }
  return -1;
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
  lval = lval_eval(env, lval);
  lenv_put(env, lval_sym, lval);
  lval_del(sexpr_args);
  return make_lval_sexpr();
}

Lval* eval_if(Lenv* env, Lval* sexpr_args) {
  LASSERT(sexpr_args, sexpr_args->count == 2 || sexpr_args->count == 3,
          "Expecting either 2 or 3 arguments to if, got %d", sexpr_args->count);
  Lval* cond = lval_pop(sexpr_args, 0);
  cond = lval_eval(env, cond);
  LASSERT(sexpr_args, cond->type == LVAL_NUM,
          "if passed incorrect type for cond, got %s, expected %s",
          lval_type_to_name(cond->type), lval_type_to_name(LVAL_NUM));
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
  LASSERT(sexpr_args, sexpr_args->count >= 1,
          "Error: lambda needs at least argument list")
  LASSERT_NODE_TYPE(sexpr_args, 0, LVAL_VECTOR, "fn");

  for (int i = 0; i < sexpr_args->node[0]->count; ++i) {
    LASSERT(sexpr_args, sexpr_args->node[0]->node[i]->type == LVAL_SYM,
            "Canot bind non-symbol. Got %s, expected %s.",
            lval_type_to_name(sexpr_args->node[0]->node[i]->type),
            lval_type_to_name(LVAL_SYM));
  }

  Lval* formals = lval_pop(sexpr_args, 0);

  Lval* ret = make_lval_lambda(formals, sexpr_args);
  lval_del(sexpr_args);
  return ret;
}

bool is_unquoted(Lval* lval) {
  return lval->type == LVAL_SEXPR && lval->count >= 2 &&
         lval->node[0]->type == LVAL_SYM &&
         strcmp(lval->node[0]->sym, "unquote") == 0;
}

bool is_splice_unquoted(Lval* lval) {
  return lval->type == LVAL_SEXPR && lval->count >= 2 &&
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
  LASSERT_IS_LIST_TYPE(ret, 1, "splice-unquote");
  return ret;
}

Lval* process_quasiquote(Lenv* env, Lval* lval) {
  Lval* processed_nodes = make_lval_sexpr();
  while (lval->count) {
    Lval* node = lval_pop(lval, 0);
    if (is_unquoted(node)) {
      Lval* unquoted_lval = eval_unquote(env, node);
      if (unquoted_lval->type == LVAL_ERR) return unquoted_lval;
      lval_add_child(processed_nodes, unquoted_lval);
    } else if (is_splice_unquoted(node)) {
      Lval* splice_unquoted_eval = eval_splice_unquote(env, node);
      if (splice_unquoted_eval->type == LVAL_ERR) return splice_unquoted_eval;
      processed_nodes = lval_concat(processed_nodes, splice_unquoted_eval);
    } else {
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
  LASSERT_NODE_COUNT(sexpr_args, 1, "quasiquote");
  Lval* arg = lval_take(sexpr_args, 0);
  Lval* ret = NULL;
  switch (arg->type) {
    case LVAL_SEXPR:
      if (is_unquoted(arg)) {
        ret = eval_unquote(env, arg);
        break;
      }
      LASSERT(sexpr_args, !is_splice_unquoted(arg),
              "Trying to splice unquote a %s. Can only use splice unquote in a "
              "quasiquoted list",
              lval_type_to_name(arg->node[1]->type));
      ret = process_quasiquote(env, arg);
      break;
    case LVAL_VECTOR:
      ret = process_quasiquote(env, arg);
      break;
    case LVAL_MAP:
      /* todo  */
    default: /* symbols, strings, numbers etc */
      ret = arg;
  }

  return ret;
}

Lval* eval_special(Lenv* env, long special_sym, Lval* sexpr_args) {
  switch (special_sym) {
    case QUOTE:
      LASSERT_NODE_COUNT(sexpr_args, 1, "quote");
      return lval_take(sexpr_args, 0);
      break;
    case QUASIQUOTE:
      return eval_quasiquote(env, sexpr_args);
      break;
    case DEF:
      return eval_def(env, sexpr_args);
      break;
    case IF:
      return eval_if(env, sexpr_args);
      break;
    case LAMBDA:
      return eval_lambda(env, sexpr_args);
      break;
    default:
      return make_lval_err("%li : not implemented yet!!\n", special_sym);
  }
  return sexpr_args;
}
