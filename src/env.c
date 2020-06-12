#include <env.h>

#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "lval.h"

Lenv* lenv_new(void) {
  Lenv* env = lalloc(LENV);
  env->parent_env = NULL;
  env->kv = NIL;
  return env;
}

void lenv_del(Lenv* e) {
  printf("OOPS!!! should be release(env) !!!!\n");
  /* if (e->kv) list_free(e->kv); */
}

int is_eq_lval_sym(void* k1, void* k2) {
  Lval* lval_k1 = k1;
  Lval* lval_k2 = k2;
  return _strcmp(lval_k1->sym, lval_k2->sym) == 0;
}

Lval* lenv_get(Lenv* env, Lval* lval_sym) {
  Lval* ret = alist_get(env->kv, is_eq_lval_sym, lval_sym);
  if (!ret && env->parent_env) {
    return lenv_get(env->parent_env, lval_sym);
  }
  /* printf("In env:\n"); */
  /* lenv_print(env); */
  /* printf("Resolved:"); */
  /* lval_println(ret); */
  return ret ? ret : make_lval_err("unbound symbol '%s'", lval_sym->sym);
}

Lenv* get_root_env(Lenv* env) {
  while (env->parent_env) {
    env = env->parent_env;
  }
  return env;
}

int lenv_is_bound(Lenv* env, Lval* lval_sym) {
  return alist_has_key(env->kv, is_eq_lval_sym, lval_sym) ? 1 : 0;
}

// Mutable
void lenv_put(Lenv* env, Lval* lval_sym, Lval* lval) {
  env->kv = alist_assoc(env->kv, is_eq_lval_sym, lval_sym, lval);
}

// Persistent
Lenv* lenv_assoc(Lenv* env, Lval* lval_sym, Lval* lval) {
  Lenv* next_env = lenv_new();
  next_env->kv = alist_passoc(env->kv, is_eq_lval_sym, lval_sym, lval);
  return next_env;
}
