#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "lval.h"
#include "mpc.h"

static void lval_expr_print(Lval* v, char open, char close);

void lval_print_str(Lval* lval) {
  char* escaped = malloc(strlen(lval->str) + 1);
  strcpy(escaped, lval->str);
  escaped = mpcf_escape(escaped);
  printf("\"%s\"", escaped);
  free(escaped);
}

void lval_print(Lval* lval) {
  switch (lval->type) {
    case LVAL_NUM:
      printf("%li", lval->num);
      break;
    case LVAL_ERR:
      printf("Error: %s", lval->err);
      break;
    case LVAL_SYM:
      printf("%s", lval->sym);
      break;
    case LVAL_STR:
      lval_print_str(lval);
      break;
    case LVAL_SEXPR:
      lval_expr_print(lval, '(', ')');
      break;
    case LVAL_MAP:
      lval_expr_print(lval, '{', '}');
      break;
    case LVAL_VECTOR:
      lval_expr_print(lval, '[', ']');
      break;
    case LVAL_FUN:
      if (lval->fun) {
        printf("<function %s>", lval->func_name);
      } else {
        printf("(fn ");
        lval_print(lval->formals);
        putchar(' ');
        lval_print(lval->body);
        putchar(')');
      }
  }
}

static void lval_expr_print(Lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {
    lval_print(v->node[i]);
    if (i != (v->count - 1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_println(Lval* v) {
  lval_print(v);
  putchar('\n');
}

void lenv_print(Lenv* e) {
  for (int i = 0; i < e->count; ++i) {
    printf("%s:", e->syms[i]);
    lval_println(e->lvals[i]);
  }
  return;
}
