#include <stdarg.h>
#include <string.h>

#include "env.h"
#include "eval.h"
#include "lval.h"
#include "print.h"
#include "read.h"

#define LASSERT(sexpr, cond, fmt, ...)        \
  if (!(cond)) {                              \
    lval* err = lval_err(fmt, ##__VA_ARGS__); \
    lval_del(sexpr);                          \
    return err;                               \
  }

#define LASSERT_ARGS_COUNT(sexpr, fn_name, expected_count) \
  LASSERT(sexpr, sexpr->count == expected_count,           \
          "Function '" fn_name                             \
          "' passed wrong number of[] args, "              \
          "got %i, expected %i",                           \
          sexpr->count, 1);

#define LASSERT_TYPE(sexpr, fn_name, index, expected_type) \
  LASSERT(sexpr, sexpr->cell[index]->type == LVAL_QEXPR,   \
          "Function '" fn_name                             \
          "' passed incorrect type for arg %d, "           \
          "got %s, expected %s",                           \
          index, ltype_name(sexpr->cell[index]->type),     \
          ltype_name(expected_type));

lval* builtin_head_fn(lenv* e, lval* sexpr) {
  LASSERT_ARGS_COUNT(sexpr, "head", 1);
  LASSERT_TYPE(sexpr, "head", 0, LVAL_QEXPR);

  lval* qexpr = lval_take(sexpr, 0);
  while (qexpr->count > 1) {
    lval_del(lval_pop(qexpr, 1));
  }
  return qexpr;
}

lval* builtin_tail_fn(lenv* e, lval* sexpr) {
  LASSERT_ARGS_COUNT(sexpr, "tail", 1);
  LASSERT_TYPE(sexpr, "tail", 0, LVAL_QEXPR);
  lval* qexpr = sexpr->cell[0];
  LASSERT(sexpr, qexpr->count != 0, "Function 'tail_fn' passed {}");

  qexpr = lval_take(sexpr, 0);
  lval_del(lval_pop(qexpr, 0));
  return qexpr;
}

lval* builtin_list_fn(lenv* e, lval* sexpr) {
  sexpr->type = LVAL_QEXPR;
  return sexpr;
}

lval* builtin_eval_fn(lenv* e, lval* sexpr) {
  LASSERT_ARGS_COUNT(sexpr, "eval", 1);
  LASSERT_TYPE(sexpr, "eval", 0, LVAL_QEXPR);
  lval* qexpr = lval_take(sexpr, 0);
  qexpr->type = LVAL_SEXPR;
  return lval_eval(e, qexpr);
}

lval* lval_join(lval* x, lval* y) {
  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  lval_del(y);
  return x;
}

lval* builtin_join_fn(lenv* e, lval* sexpr) {
  for (int i = 0; i < sexpr->count; i++) {
    LASSERT_TYPE(sexpr, "join", i, LVAL_QEXPR);
  }

  lval* qexpr = lval_pop(sexpr, 0);

  while (sexpr->count) {
    qexpr = lval_join(qexpr, lval_pop(sexpr, 0));
  }

  lval_del(sexpr);
  return qexpr;
}

static lval* builtin_op(lenv* e, char* op, lval* sexpr) {
  for (int i = 0; i < sexpr->count; i++) {
    if (sexpr->cell[i]->type != LVAL_NUM) {
      lval_del(sexpr);
      return lval_err("Expected number but got %s",
                      ltype_name(sexpr->cell[0]->type));
    }
  }
  lval* result = lval_pop(sexpr, 0);

  if ((strcmp(op, "-") == 0) && sexpr->count == 0) {
    result->num = -result->num;
  }

  while (sexpr->count > 0) {
    lval* operand = lval_pop(sexpr, 0);
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
        result = lval_err("Division by operand zero");
        break;
      }
      result->num /= operand->num;
    }

    lval_del(operand);
  }
  lval_del(sexpr);
  return result;
}

lval* builtin_add(lenv* e, lval* sexpr) { return builtin_op(e, "+", sexpr); }

lval* builtin_sub(lenv* e, lval* sexpr) { return builtin_op(e, "-", sexpr); }

lval* builtin_mul(lenv* e, lval* sexpr) { return builtin_op(e, "*", sexpr); }

lval* builtin_div(lenv* e, lval* sexpr) { return builtin_op(e, "/", sexpr); }

lval* builtin_def(lenv* e, lval* sexpr) {
  LASSERT(sexpr, sexpr->cell[0]->type == LVAL_QEXPR,
          "Function 'def' passed incorrect type");
  lval* syms = sexpr->cell[0];
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(sexpr, syms->cell[i]->type == LVAL_SYM,
            "Function 'def' cannont define a non-symbol");
  }
  LASSERT(sexpr, syms->count == sexpr->count - 1,
          "Function 'def' cannot define incorrect number of values to symbols");
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(sexpr, lenv_put(e, syms->cell[i], sexpr->cell[i + 1], false),
            "Can't redefine internal variable '%s' ", syms->cell[i]->sym);
  }
  lval_del(sexpr);
  return lval_sexpr();
}

lval* builtin_print_env(lenv* e, lval* sexpr) {
  lenv_print(e);
  lval_del(sexpr);
  return lval_sexpr();
}

bool exit_repl = false;

lval* builtin_exit(lenv* e, lval* sexpr) {
  lval_del(sexpr);
  exit_repl = true;
  return lval_sexpr();
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  lval* k = lval_sym(name);
  lval* v = lval_fun(func, name);
  lenv_put(e, k, v, true);
  lval_del(k);
  lval_del(v);
}
void lenv_add_builtins(lenv* e) {
  lenv_add_builtin(e, "list", builtin_list_fn);
  lenv_add_builtin(e, "head", builtin_head_fn);
  lenv_add_builtin(e, "tail", builtin_tail_fn);
  lenv_add_builtin(e, "eval", builtin_eval_fn);
  lenv_add_builtin(e, "join", builtin_join_fn);

  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);

  lenv_add_builtin(e, "def", builtin_def);
  lenv_add_builtin(e, "print-env", builtin_print_env);
  lenv_add_builtin(e, "exit", builtin_exit);
}
