#include "print.h"

#include "debug.h"
#include "env.h"
#include "io.h"
#include "lib.h"
#include "lval.h"
#include "mpc.h"

void lval_print_str(Lval* lval) {
  char* escaped = malloc(strlen(lval->str) + 1);
  strcpy(escaped, lval->str);
  escaped = mpcf_escape(escaped);
  printf("\"%s\"", escaped);
  free(escaped);
}

void lval_pr_str(Lval* lval) {
  char* escaped = malloc(strlen(lval->str) + 1);
  strcpy(escaped, lval->str);
  escaped = mpcf_escape(escaped);
  printf("%s", escaped);
  free(escaped);
}

void lval_collection_print(Lval* lval, char open, char close) {
  if (open) putchar(open);
  Cell* cell = lval->head;
  while (cell) {
    lval_print(cell->car);
    cell = cell->cdr;
    if (cell) putchar(' ');
  }
  if (close) putchar(close);
}

void lval_fun_print(Lval* lval) {
  switch (lval->subtype) {
    case SYS:
      printf("<function %s>", lval->func_name);
      break;
    case LAMBDA:
      printf("(fn ");
      lval_print(lval->params);
      putchar(' ');
      lval_collection_print(lval->body, 0, 0);
      putchar(')');
      break;
    case MACRO:
      printf("(macro ");
      lval_print(lval->params);
      putchar(' ');
      for (int i = 0; i < lval->body->count; i++) {
        lval_print(lval->body->node[i]);
      }
      putchar(')');
      break;
    case SPECIAL:
      printf("<function %s>", lval->func_name);
      break;
  }
}

void lval_print(Lval* lval) {
  if (!lval) {
    printf(
        "!! NULL LVAL !! NULL LVAL !! NULL LVAL !! NULL LVAL !! NULL LVAL !!");
    return;
  }
  /* printf("in lval print %s\n", lval_type_constant_to_name(lval)); */
  switch (lval->type) {
    case LVAL_SYMBOL:
      printf("%s", lval->sym);
      break;
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:
          lval_collection_print(lval, '(', ')');
          break;
        case MAP:
          lval_collection_print(lval, '{', '}');
          break;
        case VECTOR:
          lval_collection_print(lval, '[', ']');
          break;
        default:
          printf("unknown lval subtype %s\n",
                 lval_type_constant_to_name(lval->subtype));
      }
      break;
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          printf("%li", lval->num);
          break;
        case STRING:
          lval_print_str(lval);
          break;
      }
    case LVAL_FUNCTION:
      lval_fun_print(lval);
      break;
    case LVAL_ERR:
      printf("Error: %s", lval->err);
      break;
    default:
      printf("unknown lval type %d, %s\n", lval->type,
             lval_type_constant_to_name(lval->type));
  }
}

// TODO: this one prints without quotes. Make a proper pprint fn, and make
// this the normal print (so without quotes)
void lval_pr(Lval* lval) {
  if (lval->subtype == STRING) {
    lval_pr_str(lval);
    return;
  }
  lval_print(lval);
}

void lval_println(Lval* v) {
  lval_print(v);
  putchar('\n');
}

void print_kv(void* pair) {
  lval_print((Lval*)((Cell*)pair)->car);
  printf(": ");
  lval_print((Lval*)((Cell*)pair)->cdr);
}

void lenv_print(Lenv* env) {
  if (env->parent_env) {
    list_print(env->kv, print_kv, "\n");
  } else {
    printf("ROOT env!!! \n");
    /* list_print(env->kv, print_kv, "\n"); */
  }
}
