#include "lval.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"

Lval* make_lval_fun(lbuiltin func, char* func_name) {
  Lval* v = malloc(sizeof(Lval));
  v->func_name = malloc(strlen(func_name) + 1);
  strcpy(v->func_name, func_name);
  v->type = LVAL_FUN;
  v->fun = func;
  return v;
}

Lval* make_lval_lambda(Lval* formals, Lval* body) {
  Lval* lval = malloc(sizeof(Lval));
  lval->type = LVAL_FUN;
  lval->fun = NULL;

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

Lval* make_lval_sexpr(void) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->node = NULL;
  return v;
}

Lval* make_lval_qexpr(void) {
  Lval* v = malloc(sizeof(Lval));
  v->type = LVAL_QEXPR;
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
  v->node = realloc(v->node, sizeof(Lval*) * v->count);
  v->node[v->count - 1] = x;
  return v;
}

void lval_del(Lval* lval) {
  switch (lval->type) {
    case LVAL_NUM:
      break;
    case LVAL_SYM:
      free(lval->sym);
      break;
    case LVAL_ERR:
      free(lval->err);
      break;
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      for (int i = 0; i < lval->count; i++) {
        lval_del(lval->node[i]);
      }
      free(lval->node);
      break;
    case LVAL_FUN:
      if (lval->fun) {
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
  Lval* x = malloc(sizeof(Lval));
  x->type = lval->type;

  switch (lval->type) {
    case LVAL_FUN:
      if (lval->fun) {
        x->fun = lval->fun;
        x->func_name = malloc(strlen(lval->func_name) + 1);
        strcpy(x->func_name, lval->func_name);
      } else {
        x->fun = NULL;
        x->env = make_lenv_copy(lval->env);
        x->formals = make_lval_copy(lval->formals);
        x->body = make_lval_copy(lval->body);
      }
      break;
    case LVAL_NUM:
      x->num = lval->num;
      break;

    case LVAL_ERR:
      x->err = malloc(strlen(lval->err) + 1);
      strcpy(x->err, lval->err);
      break;
    case LVAL_SYM:
      x->sym = malloc(strlen(lval->sym));
      strcpy(x->sym, lval->sym);
      break;
    case LVAL_SEXPR:
    case LVAL_QEXPR:
      x->count = lval->count;
      x->node = malloc(sizeof(Lval*) * x->count);
      for (int i = 0; i < x->count; i++) {
        x->node[i] = make_lval_copy(lval->node[i]);
      }
      break;
  }
  return x;
}

Lval* lval_pop(Lval* v, int i) {
  Lval* x = v->node[i];
  memmove(&v->node[i], &v->node[i + 1], sizeof(Lval*) * (v->count - i - 1));
  v->count--;
  v->node = realloc(v->node, sizeof(Lval*) * v->count);
  return x;
}

Lval* lval_take(Lval* v, int i) {
  Lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}
