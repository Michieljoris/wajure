#ifdef WASM

#else  // not WASM
#include <env.h>

#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "state.h"

Lenv* lenv_new(void) {
  Lenv* env = lalloc_type(LENV);
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
  return _strcmp(lval_k1->data.str, lval_k2->data.str) == 0;
}

Lval* lenv_get(Lenv* env, Lval* lval_sym) {
  Lval* ret = alist_get(env->kv, is_eq_lval_sym, lval_sym);
  if (!ret && env->parent_env) {
    return lenv_get(env->parent_env, lval_sym);
  }
  return ret ? retain(ret) : NULL;
}

Lval* lenv_get_or_error(Lenv* env, Lval* lval_sym) {
  Lval* ret = alist_get(env->kv, is_eq_lval_sym, lval_sym);
  if (!ret && env->parent_env) {
    return lenv_get_or_error(env->parent_env, lval_sym);
  }
  return ret ? retain(ret)
             : make_lval_err("unbound symbol '%s'", lval_sym->data.str);
}

Lenv* get_root_env(Lenv* env) {
  while (env->parent_env) {
    env = env->parent_env;
  }
  return env;
}

Lenv* get_ns_env(Lenv* env) {
  while (env->parent_env) {
    if (env->is_ns_env) return env;
    env = env->parent_env;
  }
  printf("Error: you can't get user_env from root env!!. Aborting");
#ifndef WASM
  exit(1);
#endif
  return NULL;
}

int is_ns_env(Lenv* env) {
  return env->is_ns_env;
  /* return env->parent_env && !env->parent_env->parent_env; */
}

int lenv_is_bound(Lenv* env, Lval* lval_sym) {
  return alist_has_key(env->kv, is_eq_lval_sym, lval_sym) ? 1 : 0;
}

// Assoces lval_sym with lval in the kv list in env. Mutates env by updating kv
// list in place. Both lval_sym and lval are assumed to be retained already.
void lenv_put(Lenv* env, Lval* lval_sym, Lval* lval) {
  env->kv = alist_put(env->kv, is_eq_lval_sym, lval_sym, lval);
}

// Returns new env with sym/lval pair prepended. Does not copy over any other
// attributes. Retains head of old kv list. Both lval_sym and lval are assumed
// to be retained already.
Lenv* lenv_prepend(Lenv* env, Lval* lval_sym, Lval* lval) {
  Lenv* next_env = lenv_new();
  next_env->kv = alist_prepend(retain(env->kv), lval_sym, lval);
  next_env->parent_env = retain(env->parent_env);
  release(env);
  return next_env;
}

#endif  // WASM
