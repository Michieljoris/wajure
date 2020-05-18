#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "env.h"
#include "eval.h"
#include "lval.h"
#include "print.h"
#include "read.h"

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

Lval* eval_fn(Lenv* env, Lval* sexpr) {
  LASSERT_CELL_COUNT(sexpr, 1, "eval");
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, "eval");
  Lval* qexpr = lval_take(sexpr, 0);
  qexpr->type = LVAL_SEXPR;
  return lval_eval(env, qexpr);
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

Lval* env_put(Lenv* env, Lval* sexpr, char* fn_name, int ROOT_OR_LOCAL) {
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, fn_name)

  Lval* syms = sexpr->node[0];
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(sexpr, syms->node[i]->type == LVAL_SYM,
            "Function %s cannot define a non-symbol"
            "Got %s, expected %",
            fn_name, lval_type_to_name(syms->node[i]->type),
            lval_type_to_name(LVAL_SYM));
  }

  LASSERT(sexpr, syms->count == sexpr->count - 1,
          "Function %s cannot define incorrect number of values to symbols."
          "Got %i, expected %i",
          fn_name, syms->count, sexpr->count - 1);

  if (ROOT_OR_LOCAL == ROOT_ENV) {
    env = get_root_env(env);
  }

  for (int i = 0; i < syms->count; ++i) {
    bool is_not_internal_var = lenv_put(env, syms->node[i], sexpr->node[i + 1]);
    LASSERT(sexpr, is_not_internal_var,
            "Can't redefine internal variable '%s' ", syms->node[i]->sym);
  }
  lval_del(sexpr);
  return make_lval_sexpr();
}

Lval* def_fn(Lenv* env, Lval* sexpr) {
  return env_put(env, sexpr, "def", ROOT_ENV);
}

Lval* set_fn(Lenv* env, Lval* sexpr) {
  return env_put(env, sexpr, "set", LOCAL_ENV);
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
  LASSERT_CELL_COUNT(sexpr, 2, "fn");
  LASSERT_CELL_TYPE(sexpr, 0, LVAL_QEXPR, "fn");
  LASSERT_CELL_TYPE(sexpr, 1, LVAL_QEXPR, "fn");

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
  lenv_put_builtin(env, lval_sym, lval_fun);
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
  lenv_add_builtin(env, "set", set_fn);
  lenv_add_builtin(env, "print-env", print_env_fn);
  lenv_add_builtin(env, "exit", exit_fn);
  lenv_add_builtin(env, "fn", lambda_fn);
}
