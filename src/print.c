#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "env.h"
#include "lval.h"
#include "mpc.h"

static void lval_expr_print(Lval* v, char open, char close);

void lval_print_str(Lval* lval) {
  char* escaped = malloc(strlen(lval->str) + 1);
  strcpy(escaped, lval->str);
  escaped = mpcf_escape(escaped);
  _printf("\"%s\"", escaped);
  free(escaped);
}

void lval_pr_str(Lval* lval) {
  char* escaped = malloc(strlen(lval->str) + 1);
  strcpy(escaped, lval->str);
  escaped = mpcf_escape(escaped);
  _printf("%s", escaped);
  free(escaped);
}

void lval_fun_print(Lval* lval) {
  switch (lval->subtype) {
    case SYS:
      _printf("<function %s>", lval->func_name);
      break;
    case LAMBDA:
      _printf("(fn ");
      lval_print(lval->formals);
      _putchar(' ');
      for (int i = 0; i < lval->body->count; i++) {
        lval_print(lval->body->node[i]);
      }
      _putchar(')');
      break;
    case MACRO:
      _printf("(macro ");
      lval_print(lval->formals);
      _putchar(' ');
      for (int i = 0; i < lval->body->count; i++) {
        lval_print(lval->body->node[i]);
      }
      _putchar(')');
      break;
    case SPECIAL:
      _printf("<function %s>", lval->func_name);
      break;
  }
}

void lval_print(Lval* lval) {
  /* _printf("in lval print %s\n", lval_type_to_name2(lval)); */
  switch (lval->type) {
    case LVAL_NUM:
      _printf("%li", lval->num);
      break;
    case LVAL_ERR:
      _printf("Error: %s", lval->err);
      break;
    case LVAL_SYM:
      _printf("%s", lval->sym);
      break;
    case LVAL_STR:
      lval_print_str(lval);
      break;
    case LVAL_SEQ:
      switch (lval->subtype) {
        case LIST:
          lval_expr_print(lval, '(', ')');
          break;
        case MAP:
          lval_expr_print(lval, '{', '}');
          break;
        case VECTOR:
          lval_expr_print(lval, '[', ']');
          break;
        case PLIST:
          lval_expr_print(lval, '<', '>');
          break;
      }
      break;
    case LVAL_FUN:
      lval_fun_print(lval);
      break;
    default:
      _printf("unknown lval type %d, %s\n", lval->type,
              lval_type_to_name(lval->type));
  }
}

void lval_pr(Lval* lval) {
  /* _printf("in lval print %s\n", lval_type_to_name2(lval)); */
  switch (lval->type) {
    case LVAL_NUM:
      _printf("%li", lval->num);
      break;
    case LVAL_ERR:
      _printf("Error: %s", lval->err);
      break;
    case LVAL_SYM:
      _printf("%s", lval->sym);
      break;
    case LVAL_STR:
      lval_pr_str(lval);
      break;
    case LVAL_SEQ:
      switch (lval->subtype) {
        case LIST:
          lval_expr_print(lval, '(', ')');
          break;
        case MAP:
          lval_expr_print(lval, '{', '}');
          break;
        case VECTOR:
          lval_expr_print(lval, '[', ']');
          break;
      }
      break;
    case LVAL_FUN:
      lval_fun_print(lval);
      break;
    default:
      _printf("unknown lval type %d, %s\n", lval->type,
              lval_type_to_name(lval->type));
  }
}

static void lval_expr_print(Lval* v, char open, char close) {
  _putchar(open);
  for (int i = 0; i < v->count; i++) {
    lval_print(v->node[i]);
    if (i != (v->count - 1)) {
      _putchar(' ');
    }
  }
  _putchar(close);
}

void lval_println(Lval* v) {
  lval_print(v);
  _putchar('\n');
}

void lenv_print(Lenv* env) {
  for (int i = 0; i < env->count; ++i) {
    _printf("%s:", env->syms[i]);
    lval_println(env->lvals[i]);
  }
  return;
}
