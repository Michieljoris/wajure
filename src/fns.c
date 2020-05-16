#include <stdarg.h>
#include <string.h>

#include "env.h"
#include "eval.h"
#include "lval.h"
#include "print.h"
#include "read.h"

#define LASSERT(args, cond, fmt, ...)         \
  if (!(cond)) {                              \
    lval* err = lval_err(fmt, ##__VA_ARGS__); \
    lval_del(args);                           \
    return err;                               \
  }

#define LASSERT_ARGS_COUNT(args, fn_name, expected_count) \
  LASSERT(args, args->count == expected_count,            \
          "Function '" fn_name                            \
          "' passed wrong number of[] args, "             \
          "got %i, expected %i",                          \
          args->count, 1);

#define LASSERT_TYPE(args, fn_name, expected_type) \
  LASSERT(args, args->cell[0]->type == LVAL_QEXPR, \
          "Function '" fn_name                     \
          "' passed incorrect type, "              \
          "got %s, expected %s",                   \
          ltype_name(args->cell[0]->type), ltype_name(expected_type));

lval* builtin_head_fn(lenv* e, lval* args) {
  LASSERT_ARGS_COUNT(args, "head", 1);
  LASSERT_TYPE(args, "head", LVAL_QEXPR);

  lval* qexpr = lval_take(args, 0);
  while (qexpr->count > 1) {
    lval_del(lval_pop(qexpr, 1));
  }
  return qexpr;
}

lval* builtin_tail_fn(lenv* e, lval* args) {
  LASSERT_ARGS_COUNT(args, "tail", 1);
  LASSERT_TYPE(args, "tail", LVAL_QEXPR);
  LASSERT(args, args->cell[0]->count != 0, "Function 'tail_fn' passed {}");

  lval* qexpr = lval_take(args, 0);
  lval_del(lval_pop(qexpr, 0));
  return qexpr;
}

lval* builtin_list_fn(lenv* e, lval* sexpr) {
  sexpr->type = LVAL_QEXPR;
  return sexpr;
}

lval* builtin_eval_fn(lenv* e, lval* args) {
  LASSERT_ARGS_COUNT(args, "eval", 1);
  LASSERT_TYPE(args, "eval", LVAL_QEXPR);
  lval* qexpr = lval_take(args, 0);
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

lval* builtin_join_fn(lenv* e, lval* join_fn) {
  for (int i = 0; i < join_fn->count; i++) {
    LASSERT_TYPE(join_fn, "join", LVAL_QEXPR);
  }

  lval* qexpr = lval_pop(join_fn, 0);

  while (join_fn->count) {
    qexpr = lval_join(qexpr, lval_pop(join_fn, 0));
  }

  lval_del(join_fn);
  return qexpr;
}

static lval* builtin_op(lenv* e, char* op, lval* operands) {
  for (int i = 0; i < operands->count; i++) {
    if (operands->cell[i]->type != LVAL_NUM) {
      lval_del(operands);
      return lval_err("Expected number but got %s",
                      ltype_name(operands->cell[0]->type));
    }
  }
  lval* result = lval_pop(operands, 0);

  if ((strcmp(op, "-") == 0) && operands->count == 0) {
    result->num = -result->num;
  }

  while (operands->count > 0) {
    lval* operand = lval_pop(operands, 0);
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
  lval_del(operands);
  return result;
}

lval* builtin_add(lenv* e, lval* operands) {
  return builtin_op(e, "+", operands);
}

lval* builtin_sub(lenv* e, lval* operands) {
  return builtin_op(e, "-", operands);
}

lval* builtin_mul(lenv* e, lval* operands) {
  return builtin_op(e, "*", operands);
}

lval* builtin_div(lenv* e, lval* operands) {
  return builtin_op(e, "/", operands);
}

lval* builtin_def(lenv* e, lval* a) {
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'def' passed incorrect type");
  lval* syms = a->cell[0];
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(a, syms->cell[i]->type == LVAL_SYM,
            "Function 'def' cannont define a non-symbol");
  }
  LASSERT(a, syms->count == a->count - 1,
          "Function 'def' cannot define incorrect number of values to symbols");
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(a, lenv_put(e, syms->cell[i], a->cell[i + 1], false),
            "Can't redefine internal variable '%s' ", syms->cell[i]->sym);
  }
  lval_del(a);
  return lval_sexpr();
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  lval* k = lval_sym(name);
  lval* v = lval_fun(func, name);
  lenv_put(e, k, v, true);
  lval_del(k);
  lval_del(v);
}

lval* builtin_print_env(lenv* e, lval* operands) {
  lenv_print(e);
  lval_del(operands);
  return lval_sexpr();
}

bool exit_repl = false;

lval* builtin_exit(lenv* e, lval* operands) {
  lval_del(operands);
  exit_repl = true;
  return lval_sexpr();
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
