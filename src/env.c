#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"

Lenv* lenv_new(void) {
  Lenv* e = malloc(sizeof(Lenv));
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
}

void lenv_del(Lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}

Lval* lenv_get(Lenv* e, Lval* k) {
  for (int i = 0; i < e->count; i++) {
    if (strcmp(e->syms[i], k->sym) == 0) {
      return make_lval_copy(e->vals[i]);
    }
  }
  return make_lval_err("unbound symbol '%s'", k->sym);
}

int builtin_index_max = 0;

bool lenv_put(Lenv* e, Lval* k, Lval* v, bool is_builtin) {
  for (int i = 0; i < e->count; ++i) {
    if (strcmp(e->syms[i], k->sym) == 0) {
      if (i < builtin_index_max) {
        return false;
      }
      lval_del(e->vals[i]);
      e->vals[i] = make_lval_copy(v);
      return true;
    }
  }

  if (is_builtin) {
    builtin_index_max += 1;
  }

  e->count++;
  e->vals = realloc(e->vals, sizeof(Lval*) * e->count);
  e->syms = realloc(e->syms, sizeof(char*) * e->count);

  e->vals[e->count - 1] = make_lval_copy(v);
  e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
  strcpy(e->syms[e->count - 1], k->sym);
  return true;
}
