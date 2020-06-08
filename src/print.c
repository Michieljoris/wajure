#include "print.h"

#include "debug.h"
#include "env.h"
#include "io.h"
#include "lib.h"
#include "lval.h"
#include "mpc.h"

/* void lval_print_str(Lval* v) { */
/*   putchar('"'); */
/*   /\* Loop over the characters in the string *\/ */
/*   for (int i = 0; i < strlen(v->str); i++) { */
/*     if (strchr(lval_str_escapable, v->str[i])) { */
/*       /\* If the character is escapable then escape it *\/ */
/*       printf("%s", lval_str_escape(v->str[i])); */
/*     } else { */
/*       /\* Otherwise print character as it is *\/ */
/*       putchar(v->str[i]); */
/*     } */
/*   } */
/*   putchar('"'); */
/* } */

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

void lval_collection_print(Lval* lval, char open, char close) {
  if (open) _putchar(open);
  Cell* cell = lval->head;
  while (cell) {
    lval_print(cell->car);
    cell = cell->cdr;
    if (cell) _putchar(' ');
  }
  if (close) _putchar(close);
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
      lval_collection_print(lval->body, 0, 0);
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

void lval_print(Lval* lval) {
  /* _printf("in lval print %s\n", lval_type_constant_to_name(lval)); */
  switch (lval->type) {
    case LVAL_SYMBOL:
      _printf("%s", lval->sym);
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
          _printf("unknown lval subtype %s\n",
                  lval_type_constant_to_name(lval->subtype));
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
              lval_type_constant_to_name(lval->type));
  }
}

// TODO: this one prints without quotes. Make a proper pprint fn, and make
// this the normal print (so without quotes)
void lval_pr(Lval* lval) {
  if (lval->type == STRING) {
    lval_pr_str(lval);
    return;
  }
  lval_print(lval);
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
