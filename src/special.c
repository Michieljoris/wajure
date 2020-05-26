#include "special.h"

#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "env.h"
#include "eval.h"
#include "lval.h"

typedef struct {
  char* key;
  int val;
} t_symstruct;

enum { QUOTE, DEF, LAMBDA, IF, DEFMACRO };
static t_symstruct special_syms[] = {{"quote", QUOTE},
                                     {"def", DEF},
                                     {"fn", LAMBDA},
                                     {"if", IF},
                                     {"defmacro", DEFMACRO}};
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

  return make_lval_lambda(formals, sexpr_args);
}

Lval* eval_special(Lenv* env, long special_sym, Lval* sexpr_args) {
  switch (special_sym) {
    case QUOTE:
      LASSERT_NODE_COUNT(sexpr_args, 1, "quote");
      return sexpr_args->node[0];
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
