#include "print.h"

#include <stdio.h>

#include "env.h"
#include "lval.h"

static void lval_expr_print(lval* v, char open, char close);

static void lval_print(lval* v) {
  switch (v->type) {
    case LVAL_NUM:
      printf("%li", v->num);
      break;
    case LVAL_ERR:
      printf("Error: %s", v->err);
      break;
    case LVAL_SYM:
      printf("%s", v->sym);
      break;
    case LVAL_SEXPR:
      lval_expr_print(v, '(', ')');
      break;
    case LVAL_QEXPR:
      lval_expr_print(v, '{', '}');
      break;
    case LVAL_FUN:
      printf("<function %s>", v->func_name);
  }
}

static void lval_expr_print(lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {
    lval_print(v->cell[i]);
    if (i != (v->count - 1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_println(lval* v) {
  lval_print(v);
  putchar('\n');
}

void lenv_print(lenv* e) {
  for (int i = 0; i < e->count; ++i) {
    printf("%s:", e->syms[i]);
    lval_println(e->vals[i]);
  }
  return;
}
