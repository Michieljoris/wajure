#include "math_fns.h"

#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "env.h"
#include "lval.h"

static Lval* op_fn(Lenv* e, char* op, Lval* sexpr) {
  for (int i = 0; i < sexpr->count; i++) {
    if (sexpr->node[i]->type != LVAL_NUM) {
      lval_del(sexpr);
      return make_lval_err("Expected number but got %s",
                           lval_type_to_name(sexpr->node[0]->type));
    }
  }
  Lval* result = lval_pop(sexpr, 0);

  if ((strcmp(op, "-") == 0) && sexpr->count == 0) {
    result->num = -result->num;
  }

  while (sexpr->count > 0) {
    Lval* operand = lval_pop(sexpr, 0);
    if (strcmp(op, "+") == 0) {
      result->num += operand->num;
    }
    if (strcmp(op, "-") == 0) {
      result->num -= operand->num;
    }
    if (strcmp(op, "*") == 0) {
      result->num *= operand->num;
    }
    if (strcmp(op, "/") == 0) {
      if (operand->num == 0) {
        lval_del(result);
        lval_del(operand);
        result = make_lval_err("Division by operand zero");
        break;
      }
      result->num /= operand->num;
    }

    lval_del(operand);
  }
  lval_del(sexpr);
  return result;
}

Lval* add_fn(Lenv* e, Lval* sexpr) { return op_fn(e, "+", sexpr); }
Lval* sub_fn(Lenv* e, Lval* sexpr) { return op_fn(e, "-", sexpr); }
Lval* mul_fn(Lenv* e, Lval* sexpr) { return op_fn(e, "*", sexpr); }
Lval* div_fn(Lenv* e, Lval* sexpr) { return op_fn(e, "/", sexpr); }

#define MATH_FN(fn_name, operator_str, operator)                     \
  Lval* fn_name(Lenv* env, Lval* sexpr_args) {                       \
    LASSERT_CELL_COUNT(sexpr_args, 2, operator_str);                 \
    LASSERT_CELL_TYPE(sexpr_args, 0, LVAL_NUM, operator_str);        \
    LASSERT_CELL_TYPE(sexpr_args, 1, LVAL_NUM, operator_str);        \
    Lval* num = make_lval_num(                                       \
        sexpr_args->node[0]->num operator sexpr_args->node[1]->num); \
    lval_del(sexpr_args);                                            \
    return num;                                                      \
  }

MATH_FN(gt_fn, ">", >);
MATH_FN(lt_fn, "<", <);
MATH_FN(lte_fn, "<=", <=);
MATH_FN(gte_fn, ">=", >=);

int lval_eq(Lval* x, Lval* y) {
  if (x->type != y->type) {
    return 0;
  }

  switch (x->type) {
    case LVAL_NUM:
      return (x->num == y->num);
    case LVAL_ERR:
      return (strcmp(x->err, y->err) == 0);
    case LVAL_SYM:
      return (strcmp(x->sym, y->sym) == 0);
    case LVAL_FUN:
      if (x->fun || y->fun) {
        return x->fun == y->fun;
      } else {
        return lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body);
      }
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      if (x->count != y->count) {
        return 0;
      }
      for (int i = 0; i < x->count; ++i) {
        if (!lval_eq(x->node[i], y->node[i])) {
          return 0;
        }
      }
      return 1;
  }
  printf("Warning: comparing instances of type '%s' is not implemented\n",
         lval_type_to_name(x->type));
  return 0;
}

Lval* cmp_fn(Lenv* env, Lval* sexpr_args, char* operator) {
  LASSERT_CELL_COUNT(sexpr_args, 2, operator);
  int result;
  if (strcmp(operator, "=") == 0) {
    result = lval_eq(sexpr_args->node[0], sexpr_args->node[1]);
  }

  if (strcmp(operator, "not=") == 0) {
    result = !lval_eq(sexpr_args->node[0], sexpr_args->node[1]);
  }
  lval_del(sexpr_args);
  return make_lval_num(result);
}

Lval* eq_fn(Lenv* env, Lval* sexpr_args) {
  return cmp_fn(env, sexpr_args, "=");
}

Lval* not_eq_fn(Lenv* env, Lval* sexpr_args) {
  return cmp_fn(env, sexpr_args, "not=");
}

void lenv_add_math_fns(Lenv* env) {
  lenv_add_builtin(env, "+", add_fn);
  lenv_add_builtin(env, "-", sub_fn);
  lenv_add_builtin(env, "*", mul_fn);
  lenv_add_builtin(env, "/", div_fn);

  lenv_add_builtin(env, ">", gt_fn);
  lenv_add_builtin(env, "<", lt_fn);
  lenv_add_builtin(env, ">=", gte_fn);
  lenv_add_builtin(env, "<=", lte_fn);

  lenv_add_builtin(env, "=", eq_fn);
  lenv_add_builtin(env, "not=", not_eq_fn);
}
