#include <env.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"

Lenv* lenv_new(void) {
  Lenv* env = malloc(sizeof(Lenv));
  env->parent_env = NULL;
  env->count = 0;
  env->syms = NULL;
  env->lvals = NULL;
  return env;
}

Lenv* make_lenv_copy(Lenv* env) {
  Lenv* env_copy = malloc(sizeof(Lenv));
  env_copy->parent_env = env->parent_env;
  env_copy->count = env->count;
  env_copy->syms = malloc(sizeof(char*) * env->count);
  env_copy->lvals = malloc(sizeof(Lval*) * env->count);
  for (int i = 0; i < env->count; ++i) {
    env_copy->syms[i] = malloc(sizeof(strlen(env_copy->syms[i]) + 1));
    strcpy(env_copy->syms[i], env->syms[i]);
    env_copy->lvals[i] = make_lval_copy(env->lvals[i]);
  }
  return env_copy;
}

void lenv_del(Lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->lvals[i]);
  }
  free(e->syms);
  free(e->lvals);
  free(e);
}

Lval* lenv_get(Lenv* env, Lval* lval_sym) {
  for (int i = 0; i < env->count; i++) {
    if (strcmp(env->syms[i], lval_sym->sym) == 0) {
      return make_lval_copy(env->lvals[i]);
    }
  }
  if (env->parent_env) {
    return lenv_get(env->parent_env, lval_sym);
  }
  return make_lval_err("unbound symbol '%s'", lval_sym->sym);
}

int builtin_index_max = 0;

Lenv* get_root_env(Lenv* env) {
  while (env->parent_env) {
    env = env->parent_env;
  }
  return env;
}

int find_str_index(char** strs, int count, char* str) {
  for (int i = 0; i < count; ++i) {
    if (strcmp(strs[i], str) == 0) {
      return i;
    }
  }
  return -1;
}

bool lenv_put(Lenv* env, Lval* lval_sym, Lval* lval, int var_type) {
  char* sym = lval_sym->sym;
  Lenv* root_env = get_root_env(env);
  int sym_index = find_str_index(root_env->syms, root_env->count, sym);
  bool is_root_env = env->parent_env == NULL;

  if (var_type == BUILTIN) {
    builtin_index_max += 1;
  } else {
    /* Check if already bound in the root env*/
    if (sym_index != -1) {
      if (sym_index < builtin_index_max) {
        return false;
      }
      if (!is_root_env) {
        sym_index = find_str_index(env->syms, env->count, sym);
      }
      if (sym_index != -1) {
        lval_del(env->lvals[sym_index]);
        env->lvals[sym_index] = make_lval_copy(lval);
        return true;
      }
    }
  }

  env->count++;
  env->lvals = realloc(env->lvals, sizeof(Lval*) * env->count);
  env->syms = realloc(env->syms, sizeof(char*) * env->count);

  env->lvals[env->count - 1] = make_lval_copy(lval);
  env->syms[env->count - 1] = malloc(strlen(lval_sym->sym) + 1);
  strcpy(env->syms[env->count - 1], lval_sym->sym);
  return true;
}
