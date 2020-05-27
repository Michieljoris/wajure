#include "lval.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "print.h"

Lval* make_lval_fun(lbuiltin func, char* func_name, int subtype) {
  Lval* lval = malloc(sizeof(Lval));
  lval->func_name = malloc(strlen(func_name) + 1);
  strcpy(lval->func_name, func_name);
  lval->type = LVAL_FUN;
  lval->subtype = subtype;
  lval->fun = func;
  return lval;
}

Lval* make_lval_special(lbuiltin func, char* func_name) {
  Lval* lval = malloc(sizeof(Lval));
  lval->func_name = malloc(strlen(func_name) + 1);
  strcpy(lval->func_name, func_name);
  lval->type = LVAL_FUN;
  lval->subtype = SPECIAL;
  lval->fun = func;
  return lval;
}

Lval* make_lval_lambda(Lval* formals, Lval* body) {
  Lval* lval = malloc(sizeof(Lval));
  lval->type = LVAL_FUN;
  lval->subtype = LAMBDA;

  lval->env = lenv_new();
  lval->formals = formals;
  lval->body = body;
  return lval;
}

Lval* make_lval_macro(Lval* formals, Lval* body) {
  Lval* lval = malloc(sizeof(Lval));
  lval->type = LVAL_FUN;
  lval->subtype = MACRO;

  lval->env = lenv_new();
  lval->formals = formals;
  lval->body = body;
  return lval;
}

Lval* make_lval_num(long x) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

Lval* make_lval_sym(char* s) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

Lval* make_lval_str(char* s) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_STR;
  v->str = malloc(strlen(s) + 1);
  strcpy(v->str, s);
  return v;
}

Lval* make_lval_sexpr(void) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_SEQ;
  v->subtype = LIST;
  v->count = 0;
  v->node = NULL;
  return v;
}

Lval* make_lval_vector(void) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_SEQ;
  v->subtype = VECTOR;
  v->count = 0;
  v->node = NULL;
  return v;
}

Lval* make_lval_map(void) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_SEQ;
  v->subtype = MAP;
  v->count = 0;
  v->node = NULL;
  return v;
}

Lval* make_lval_quote(void) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_QUOTE;
  v->count = 0;
  v->node = NULL;
  return v;
}

Lval* make_lval_err(char* fmt, ...) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_ERR;

  va_list va;
  va_start(va, fmt);
  v->err = malloc(512);
  vsnprintf(v->err, 511, fmt, va);
  v->err = realloc(v->err, strlen(v->err) + 1);

  va_end(va);
  return v;
}

char* lval_type_to_name2(Lval* lval) {
  switch (lval->type) {
    case LVAL_NUM:
      return "Number";
    case LVAL_SYM:
      return "Symbol";
    case LVAL_SEQ:
      switch (lval->subtype) {
        case LIST:
          return "List";
        case VECTOR:
          return "Vector";
        case MAP:
          return "Map";
        default:
          return "unknown SEQ subtype";
      }
    case LVAL_ERR:
      return "Error";
    case LVAL_FUN:
      switch (lval->subtype) {
        case SYS:
          return "System Function";
        case LAMBDA:
          return "User Function";
        case SPECIAL:
          return "Special form";
        case MACRO:
          return "Macro";
      }
    case LVAL_STR:
      return "String";
    default:
      return "Unknown";
  }
}

char* lval_type_to_name(int t) {
  switch (t) {
    case LVAL_NUM:
      return "Number";
    case LVAL_SYM:
      return "Symbol";
    case LVAL_SEQ:
      return "Seq";
    case LVAL_ERR:
      return "Error";
    case LVAL_FUN:
      return "Function";
    case LVAL_STR:
      return "String";
    case LIST:
      return "List";
    case VECTOR:
      return "List";
    case MAP:
      return "List";
    default:
      return "Unknown";
  }
}

Lval* lval_add_child(Lval* lval, Lval* x) {
  lval->count++;
  lval->node = realloc(lval->node, sizeof(Lval*) * lval->count);
  lval->node[lval->count - 1] = x;
  return lval;
}

void lval_del(Lval* lval) {
  switch (lval->type) {
    case LVAL_NUM:
      break;
    case LVAL_STR:
      free(lval->str);
      break;
    case LVAL_SYM:
      free(lval->sym);
      break;
    case LVAL_ERR:
      free(lval->err);
      break;
    case LVAL_SEQ:
      for (int i = 0; i < lval->count; i++) {
        lval_del(lval->node[i]);
      }
      free(lval->node);
      break;
    case LVAL_FUN:
      if (lval->subtype == SYS || lval->subtype == SPECIAL) {
        free(lval->func_name);
      } else {
        lenv_del(lval->env);
        lval_del(lval->formals);
        lval_del(lval->body);
      }
      break;
  }
  free(lval);
}

Lval* make_lval_copy(Lval* lval) {
  /* printf("make_lval_copy\n"); */
  Lval* x = malloc(sizeof(Lval));
  /* printf("make_lval_copy\n"); */
  /* lval_println(lval); */
  x->type = lval->type;
  x->subtype = lval->subtype;
  /* printf("switching\n"); */
  switch (lval->type) {
    case LVAL_FUN:
      if (lval->subtype == SYS || lval->subtype == SPECIAL) {
        x->fun = lval->fun;
        x->func_name = malloc(strlen(lval->func_name) + 1);
        strcpy(x->func_name, lval->func_name);
      } else {
        /* printf("in lval_copy\n"); */
        /* lval_println(lval); */
        x->env = make_lenv_copy(lval->env);
        /* printf("copied ENV\n"); */
        x->formals = make_lval_copy(lval->formals);
        /* printf("copied FORMALS\n"); */
        /* lval_println(lval->body); */
        /* printf("%s\n", lval_type_to_name(lval->body->type)); */
        x->body = make_lval_copy(lval->body);
        /* printf("copied BODY\n"); */
      }
      break;
    case LVAL_NUM:
      x->num = lval->num;
      break;

    case LVAL_ERR:
      x->err = malloc(strlen(lval->err) + 1);
      strcpy(x->err, lval->err);
      break;
    case LVAL_STR:
      x->str = malloc(strlen(lval->str) + 1);
      strcpy(x->str, lval->str);
      break;
    case LVAL_SYM:
      x->sym = malloc(strlen(lval->sym) + 1);
      strcpy(x->sym, lval->sym);
      break;
    case LVAL_SEQ:
      /* printf("copying seq\n"); */
      x->count = lval->count;
      x->node = malloc(sizeof(Lval*) * x->count);
      for (int i = 0; i < x->count; i++) {
        x->node[i] = make_lval_copy(lval->node[i]);
      }
      break;

    default:
      printf("Don't know how to print %s\n", lval_type_to_name2(lval));
  }
  return x;
}

Lval* lval_pop(Lval* lval, int i) {
  Lval* x = lval->node[i];
  memmove(&lval->node[i], &lval->node[i + 1],
          sizeof(Lval*) * (lval->count - i - 1));
  lval->count--;
  lval->node = realloc(lval->node, sizeof(Lval*) * lval->count);
  return x;
}

Lval* lval_take(Lval* v, int i) {
  Lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

Lval* lval_concat(Lval* x, Lval* y) {
  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add_child(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  lval_del(y);
  return x;
}
