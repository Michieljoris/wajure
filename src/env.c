#include <env.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "lispy_mempool.h"
#include "lval.h"

Lenv* lenv_new(void) {
  Lenv* env = lalloc(LENV);
  env->parent_env = NULL;
  env->kv = list_new();
  return env;
}

// Mutable
Lenv* lenv_copy(Lenv* env) {
  Lenv* env_copy = lalloc(LENV);
  env_copy->parent_env = env->parent_env;
  env_copy->kv = list_copy(env->kv, NULL);
  return env_copy;
}

// Persistent;
Lenv* lenv_pcopy(Lenv* env) {
  Lenv* env_copy = lalloc(LENV);
  env_copy->parent_env = env->parent_env;
  env_copy->kv = env->kv;
  return env_copy;
}

void lenv_del(Lenv* e) {
  list_free(e->kv);
  lfree(LENV, e);
}

int is_eq_str(void* k, void* v) { return strcmp((char*)k, (char*)v) == 0; }

Lval* lenv_get(Lenv* env, Lval* lval_sym) {
  Lval* ret = alist_get(env->kv, is_eq_str, lval_sym->sym);
  if (!ret && env->parent_env) {
    return lenv_get(env->parent_env, lval_sym);
  }
  return ret ? make_lval_copy(ret)
             : make_lval_err("unbound symbol '%s'", lval_sym->sym);
}

Lenv* get_root_env(Lenv* env) {
  while (env->parent_env) {
    env = env->parent_env;
  }
  return env;
}

int lenv_is_bound(Lenv* env, Lval* lval_sym) {
  return alist_has_key(env->kv, is_eq_str, lval_sym->sym) ? 1 : 0;
}

// Mutable
void lenv_put(Lenv* env, Lval* lval_sym, Lval* lval) {
  alist_assoc(env->kv, is_eq_str, lval_sym->sym, lval);
}

// Persistent
void lenv_assoc(Lenv* env, Lval* lval_sym, Lval* lval) {
  Cell* kv = alist_passoc(env->kv, is_eq_str, lval_sym->sym, lval);
  list_free(env->kv);
  env->kv = kv;
}

void lenv_add_builtin(Lenv* env, char* name, lbuiltin func, int subtype) {
  Lval* lval_sym = make_lval_sym(name);
  if (lenv_is_bound(get_root_env(env), lval_sym)) {
    printf("Warning: duplicate builtin fn: '%s'\n", lval_sym->sym);
  }
  Lval* lval_fun = make_lval_fun(func, name, subtype);
  lenv_put(env, lval_sym, lval_fun);
}
