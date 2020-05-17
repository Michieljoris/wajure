#include "lval.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Lval* make_lval_fun(lbuiltin func, char* func_name) {
  Lval* v = malloc(sizeof(Lval));
  v->func_name = malloc(strlen(func_name) + 1);
  strcpy(v->func_name, func_name);
  v->type = LVAL_FUN;
  v->fun = func;
  return v;
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

Lval* make_lval_sexpr(void) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

Lval* make_lval_qexpr(void) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_QEXPR;
  v->count = 0;
  v->cell = NULL;
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

char* lval_type_to_name(int t) {
  switch (t) {
    case LVAL_NUM:
      return "Number";
    case LVAL_SYM:
      return "Symbol";
    case LVAL_QEXPR:
      return "Q-Expression";
    case LVAL_SEXPR:
      return "S-Expression";
    case LVAL_ERR:
      return "Error";
    case LVAL_FUN:
      return "Function";
    default:
      return "Unknown";
  }
}

Lval* lval_add_child(Lval* v, Lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(Lval*) * v->count);
  v->cell[v->count - 1] = x;
  return v;
}

void lval_del(Lval* v) {
  switch (v->type) {
    case LVAL_NUM:
      break;
    case LVAL_SYM:
      free(v->sym);
      break;
    case LVAL_ERR:
      free(v->err);
      break;
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      for (int i = 0; i < v->count; i++) {
        lval_del(v->cell[i]);
      }
      free(v->cell);
      break;
    case LVAL_FUN:
      free(v->func_name);
      break;
  }
  free(v);
}

Lval* make_lval_copy(Lval* v) {
  Lval* x = malloc(sizeof(Lval));
  x->type = v->type;

  switch (v->type) {
    case LVAL_FUN:
      x->fun = v->fun;
      x->func_name = malloc(strlen(v->func_name) + 1);
      strcpy(x->func_name, v->func_name);
      break;
    case LVAL_NUM:
      x->num = v->num;
      break;

    case LVAL_ERR:
      x->err = malloc(strlen(v->err) + 1);
      strcpy(x->err, v->err);
      break;
    case LVAL_SYM:
      x->sym = malloc(strlen(v->sym));
      strcpy(x->sym, v->sym);
      break;
    case LVAL_SEXPR:
    case LVAL_QEXPR:
      x->count = v->count;
      x->cell = malloc(sizeof(Lval*) * x->count);
      for (int i = 0; i < x->count; i++) {
        x->cell[i] = make_lval_copy(v->cell[i]);
      }
      break;
  }
  return x;
}

Lval* lval_pop(Lval* v, int i) {
  Lval* x = v->cell[i];
  memmove(&v->cell[i], &v->cell[i + 1], sizeof(Lval*) * (v->count - i - 1));
  v->count--;
  v->cell = realloc(v->cell, sizeof(Lval*) * v->count);
  return x;
}

Lval* lval_take(Lval* v, int i) {
  Lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}
