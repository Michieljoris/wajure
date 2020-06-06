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
      lval_print(lval->params);
      _putchar(' ');
      for (int i = 0; i < lval->body->count; i++) {
        lval_print(lval->body->node[i]);
      }
      _putchar(')');
      break;
    case MACRO:
      _printf("(macro ");
      lval_print(lval->params);
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

void lval_plist_print(Lval* lval) {
  _putchar('(');
  Cell* cell = lval->cell;
  while (cell) {
    lval_print(cell->car);
    cell = cell->cdr;
    if (cell) _putchar(' ');
  }
  _putchar(')');
}

void lval_print(Lval* lval) {
  /* _printf("in lval print %s\n", lval_type_to_name2(lval)); */
  switch (lval->type) {
    case LVAL_SYMBOL:
      _printf("%s", lval->sym);
      break;
    case LVAL_COLLECTION:
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
          lval_plist_print(lval);
          break;
        default:
          _printf("unknown lval subtype %s\n",
                  lval_type_to_name(lval->subtype));
      }
      break;
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          _printf("%li", lval->num);
          break;
        case STRING:
          lval_print_str(lval);
          break;
      }
    case LVAL_FUNCTION:
      lval_fun_print(lval);
      break;
    case LVAL_ERR:
      _printf("Error: %s", lval->err);
      break;
    default:
      _printf("unknown lval type %d, %s\n", lval->type,
              lval_type_to_name(lval->type));
  }
}

// TODO: this one prints without quotes. Make a proper pprint fn, and make
// this the normal print (so without quotes)
void lval_pr(Lval* lval) {
  /* _printf("in lval print %s\n", lval_type_to_name2(lval)); */
  switch (lval->type) {
    case LVAL_SYMBOL:
      _printf("%s", lval->sym);
      break;
    case LVAL_COLLECTION:
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
          lval_plist_print(lval);
          break;
        default:
          _printf("unknown lval subtype %s\n",
                  lval_type_to_name(lval->subtype));
      }
      break;
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          _printf("%li", lval->num);
          break;
        case STRING:
          lval_pr_str(lval);
          break;
      }
    case LVAL_FUNCTION:
      lval_fun_print(lval);
      break;
    case LVAL_ERR:
      _printf("Error: %s", lval->err);
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

void print_kv(void* pair) {
  printf("%s: ", (char*)((Cell*)pair)->car);
  lval_print((Lval*)((Cell*)pair)->cdr);
}

void lenv_print(Lenv* env) {
  printf("In lenv_print:\n");
  if (env->parent_env) {
    list_print(env->kv, print_kv, "\n");
  } else {
    printf("ROOT env!!! \n");
  }
}
