#include "eval.h"

#include <stdlib.h>
#include <string.h>

#include "lval.h"

lval* lval_eval(lval* v);

static lval* lval_pop(lval* v, int i) {
  lval* x = v->cell[i];
  memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval*) * (v->count - i - 1));
  v->count--;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}

static lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

#define LASSERT(args, cond, err) \
  if (!(cond)) {                 \
    lval_del(args);              \
    return lval_err(err);        \
  }

lval* builtin_head_fn(lval* args) {
  LASSERT(args, args->count == 1, "Function 'head_fn' passed too many args");
  LASSERT(args, args->cell[0]->type == LVAL_QEXPR,
          "Function 'head_fn' passed incorrect type");
  LASSERT(args, args->cell[0]->count != 0, "Function 'head_fn' passed {}");

  lval* qexpr = lval_take(args, 0);
  while (qexpr->count > 1) {
    lval_del(lval_pop(qexpr, 1));
  }
  return qexpr;
}

lval* builtin_tail_fn(lval* args) {
  LASSERT(args, args->count == 1, "Function 'tail_fn' passed too many args");
  LASSERT(args, args->cell[0]->type == LVAL_QEXPR,
          "Function 'tail_fn' passed incorrect type");
  LASSERT(args, args->cell[0]->count != 0, "Function 'tail_fn' passed {}");

  lval* qexpr = lval_take(args, 0);
  lval_del(lval_pop(qexpr, 0));
  return qexpr;
}

lval* builtin_list_fn(lval* sexpr) {
  sexpr->type = LVAL_QEXPR;
  return sexpr;
}

lval* builtin_eval(lval* args) {
  LASSERT(args, args->count == 1, "Function 'eval' passed too many args");
  LASSERT(args, args->cell[0]->type == LVAL_QEXPR,
          "Function 'eval' passed incorrect type");
  lval* qexpr = lval_take(args, 0);
  qexpr->type = LVAL_SEXPR;
  return lval_eval(qexpr);
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

lval* builtin_join(lval* join_fn) {
  for (int i = 0; i < join_fn->count; i++) {
    LASSERT(join_fn, join_fn->cell[i]->type == LVAL_QEXPR,
            "Function 'join' passed incorrect type.");
  }

  lval* qexpr = lval_pop(join_fn, 0);

  while (join_fn->count) {
    qexpr = lval_join(qexpr, lval_pop(join_fn, 0));
  }

  lval_del(join_fn);
  return qexpr;
}

static lval* builtin_op(char* op, lval* operands) {
  for (int i = 0; i < operands->count; ++i) {
    if (operands->cell[i]->type != LVAL_NUM) {
      lval_del(operands);
      return lval_err("Cannot operate on non-number");
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
        result = lval_err("Division boperand zero");
        break;
      }
      result->num /= operand->num;
    }

    lval_del(operand);
  }
  lval_del(operands);
  return result;
}

lval* builtin(char* func, lval* args) {
  if (strcmp("list", func) == 0) {
    return builtin_list_fn(args);
  }
  if (strcmp("head", func) == 0) {
    return builtin_head_fn(args);
  }
  if (strcmp("tail", func) == 0) {
    return builtin_tail_fn(args);
  }
  if (strcmp("join", func) == 0) {
    return builtin_join(args);
  }
  if (strcmp("eval", func) == 0) {
    return builtin_eval(args);
  }
  if (strstr("+-/*", func)) {
    return builtin_op(func, args);
  }
  lval_del(args);
  return lval_err("Unknown Function!");
}

static lval* lval_eval_sexpr(lval* v) {
  /* eval children */
  for (int i = 0; i < v->count; ++i) {
    v->cell[i] = lval_eval(v->cell[i]);
  }
  /* error checking */
  for (int i = 0; i < v->count; ++i) {
    if (v->cell[i]->type == LVAL_ERR) {
      return lval_take(v, i);
    }
  }
  /* empty expr */
  if (v->count == 0) {
    return v;
  }
  /* single expr */
  if (v->count == 1) {
    return lval_take(v, 0);
  }
  /* first element should be a symbol  */
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_SYM) {
    lval_del(f);
    lval_del(v);
    return lval_err("sexpr doesn't start with a symbol");
  }
  /* call builtin with operator */
  lval* result = builtin(f->sym, v);
  lval_del(f);
  return result;
}

lval* lval_eval(lval* v) {
  if (v->type == LVAL_SEXPR) {
    return lval_eval_sexpr(v);
  }
  return v;
}
