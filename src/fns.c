#include <stdarg.h>
#include <string.h>

#include "env.h"
#include "eval.h"
#include "lval.h"
#include "print.h"
#include "read.h"

#define LASSERT(sexpr, cond, fmt, ...)             \
  if (!(cond)) {                                   \
    Lval* err = make_lval_err(fmt, ##__VA_ARGS__); \
    lval_del(sexpr);                               \
    return err;                                    \
  }

#define LASSERT_CELL_COUNT(sexpr, expected_count, fn_name) \
  LASSERT(sexpr, sexpr->count == expected_count,           \
          "Function '" fn_name                             \
          "' passed wrong number of[] args, "              \
          "got %i, expected %i",                           \
          sexpr->count, 1);

#define LASSERT_CELL_TYPE(sexpr, index, expected_type, fn_name) \
  LASSERT(sexpr, sexpr->node[index]->type == LVAL_QEXPR,        \
          "Function '" fn_name                                  \
          "' passed incorrect type for arg %d, "                \
          "got %s, expected %s",                                \
          index, lval_type_to_name(sexpr->node[index]->type),   \
          lval_type_to_name(expected_type));

Lval* head_fn(Lenv* e, Lval* sexpr) {
  LASSERT_CELL_COUNT(sexpr, 1, "head");
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, "head");

  Lval* qexpr = lval_take(sexpr, 0);
  while (qexpr->count > 1) {
    lval_del(lval_pop(qexpr, 1));
  }
  return qexpr;
}

Lval* tail_fn(Lenv* e, Lval* sexpr) {
  LASSERT_CELL_COUNT(sexpr, 1, "tail");
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, "tail");
  Lval* qexpr = sexpr->node[0];
  LASSERT(sexpr, qexpr->count != 0, "Function 'tail_fn' passed {}");

  qexpr = lval_take(sexpr, 0);
  lval_del(lval_pop(qexpr, 0));
  return qexpr;
}

Lval* list_fn(Lenv* e, Lval* sexpr) {
  sexpr->type = LVAL_QEXPR;
  return sexpr;
}

Lval* eval_fn(Lenv* e, Lval* sexpr) {
  LASSERT_CELL_COUNT(sexpr, 1, "eval");
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, "eval");
  Lval* qexpr = lval_take(sexpr, 0);
  qexpr->type = LVAL_SEXPR;
  return lval_eval(e, qexpr);
}

Lval* lval_join(Lval* x, Lval* y) {
  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add_child(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  lval_del(y);
  return x;
}

Lval* join_fn(Lenv* e, Lval* sexpr) {
  for (int i = 0; i < sexpr->count; i++) {
    LASSERT_CELL_TYPE(sexpr, i, LVAL_QEXPR, "join");
  }

  Lval* qexpr = lval_pop(sexpr, 0);

  while (sexpr->count) {
    qexpr = lval_join(qexpr, lval_pop(sexpr, 0));
  }

  lval_del(sexpr);
  return qexpr;
}

static Lval* _op(Lenv* e, char* op, Lval* sexpr) {
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

Lval* add_fn(Lenv* e, Lval* sexpr) { return _op(e, "+", sexpr); }

Lval* sub_fn(Lenv* e, Lval* sexpr) { return _op(e, "-", sexpr); }

Lval* mul_fn(Lenv* e, Lval* sexpr) { return _op(e, "*", sexpr); }

Lval* div_fn(Lenv* e, Lval* sexpr) { return _op(e, "/", sexpr); }

Lval* def_fn(Lenv* e, Lval* sexpr) {
  LASSERT(sexpr, sexpr->node[0]->type == LVAL_QEXPR,
          "Function 'def' passed incorrect type");
  Lval* syms = sexpr->node[0];
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(sexpr, syms->node[i]->type == LVAL_SYM,
            "Function 'def' cannont define a non-symbol");
  }
  LASSERT(sexpr, syms->count == sexpr->count - 1,
          "Function 'def' cannot define incorrect number of values to symbols");
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(sexpr, lenv_put(e, syms->node[i], sexpr->node[i + 1], false),
            "Can't redefine internal variable '%s' ", syms->node[i]->sym);
  }
  lval_del(sexpr);
  return make_lval_sexpr();
}

Lval* print_env_fn(Lenv* e, Lval* sexpr) {
  lenv_print(e);
  lval_del(sexpr);
  return make_lval_sexpr();
}

bool exit_repl = false;

Lval* exit_fn(Lenv* e, Lval* sexpr) {
  lval_del(sexpr);
  exit_repl = true;
  return make_lval_sexpr();
}

Lval* lambda_fn(Lenv* env, Lval* sexpr) {
  LASSERT_CELL_COUNT(sexpr, 2, "\\");
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, "\\");
  LASSERT_CELL_TYPE(sexpr, 1, LVAL_QEXPR, "\\");

  for (int i = 0; i < sexpr->node[0]->count; ++i) {
    LASSERT(sexpr, sexpr->node[0]->node[i]->type == LVAL_SYM,
            "Canot define non-symbol. Got %s, expected %s.",
            lval_type_to_name(sexpr->node[0]->node[i]->type),
            lval_type_to_name(LVAL_SYM));
  }

  Lval* formals = lval_pop(sexpr, 0);
  Lval* body = lval_pop(sexpr, 0);
  lval_del(sexpr);

  return make_lval_lambda(formals, body);
}

void lenv_add_builtin(Lenv* env, char* name, lbuiltin func) {
  Lval* lval_sym = make_lval_sym(name);
  Lval* lval_fun = make_lval_fun(func, name);
  lenv_put(env, lval_sym, lval_fun, true);
  lval_del(lval_sym);
  lval_del(lval_fun);
}
void lenv_add_builtins(Lenv* env) {
  lenv_add_builtin(env, "list", list_fn);
  lenv_add_builtin(env, "head", head_fn);
  lenv_add_builtin(env, "tail", tail_fn);
  lenv_add_builtin(env, "eval", eval_fn);
  lenv_add_builtin(env, "join", join_fn);

  lenv_add_builtin(env, "+", add_fn);
  lenv_add_builtin(env, "-", sub_fn);
  lenv_add_builtin(env, "*", mul_fn);
  lenv_add_builtin(env, "/", div_fn);

  lenv_add_builtin(env, "def", def_fn);
  lenv_add_builtin(env, "print-env", print_env_fn);
  lenv_add_builtin(env, "exit", exit_fn);
  lenv_add_builtin(env, "\\", lambda_fn);
}
