#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"

Lenv* lenv_new(void) {
  Lenv* env = malloc(sizeof(Lenv));
  env->parent_env = NULL;
  env->count = 0;
  env->syms = NULL;
  env->vals = NULL;
  return env;
}

Lenv* lenv_copy(Lenv* env) {
  Lenv* env_copy = malloc(sizeof(Lenv));
  env_copy->parent_env = env->parent_env;
  env_copy->count = env->count;
  env_copy->syms = malloc(sizeof(char*) * env->count);
  env_copy->vals = malloc(sizeof(Lval*) * env->count);
  for (int i = 0; i < env->count; ++i) {
    env_copy->syms[i] = malloc(sizeof(strlen(env_copy->syms[i]) + 1));
    strcpy(env_copy->syms[i], env->syms[i]);
    env_copy->vals[i] = make_lval_copy(env->vals[i]);
  }
  return env_copy;
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

Lval* lenv_get(Lenv* env, Lval* lval_sym) {
  for (int i = 0; i < env->count; i++) {
    if (strcmp(env->syms[i], lval_sym->sym) == 0) {
      return make_lval_copy(env->vals[i]);
    }
  }
  if (env->parent_env) {
    return lenv_get(env->parent_env, lval_sym);
  }
  return make_lval_err("unbound symbol '%s'", lval_sym->sym);
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
