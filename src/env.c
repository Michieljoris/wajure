#include <env.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_mempool.h"
#include "mempool.h"
#include "print.h"

/* Lenv* lenv_new(void) { */
/*   Lenv* env = malloc(sizeof(Lenv)); */
/*   env->parent_env = NULL; */
/*   env->count = 0; */
/*   env->syms = NULL; */
/*   env->lvals = NULL; */
/*   return env; */
/* } */

Lenv* lenv_new(void) {
  Lenv* env = mempool_alloc(lval_mempool);
  env->parent_env = NULL;
  env->kv = NULL;
  return env;
}

Lenv* make_lenv_copy(Lenv* env) {
  Lenv* env_copy = mempool_alloc(lval_mempool);
  env_copy->parent_env = env->parent_env;

  // persistent;
  env_copy->kv = env->kv;
  return env_copy;
  // mutable

  /*   env_copy->count = env->count; */
  /*   /\* env_copy->syms = malloc(sizeof(char*) * env->count); *\/ */
  /*   /\* env_copy->lvals = malloc(sizeof(Lval*) * env->count); *\/ */
  /*   for (int i = 0; i < env->count; ++i) { */
  /*     env_copy->syms[i] = malloc(sizeof(strlen(env_copy->syms[i]) + 1)); */
  /*     strcpy(env_copy->syms[i], env->syms[i]); */
  /*     env_copy->lvals[i] = make_lval_copy(env->lvals[i]); */
  /*   } */
  /*   return env_copy; */
  return env;
}

void lenv_del(Lenv* e) {
  /* for (int i = 0; i < e->count; i++) { */
  /*   free(e->syms[i]); */
  /*   lval_del(e->lvals[i]); */
  /* } */
  /* free(e->syms); */
  /* free(e->lvals); */
  /* free(e); */
}

Lval* lenv_get(Lenv* env, Lval* lval_sym) {
  Lval* ret = NULL;
  while (env->kv) {
    Lval* lval = env->kv->car;
    env->kv = env->kv->cdr;
    if (strcmp(lval->sym, lval_sym->sym) == 0) {
      ret = env->kv->car;
      break;
    }
  }
  if (!ret && env->parent_env) {
    return lenv_get(env->parent_env, lval_sym);
  }
  return ret ? ret : make_lval_err("unbound symbol '%s'", lval_sym->sym);
}

Lenv* get_root_env(Lenv* env) {
  while (env->parent_env) {
    env = env->parent_env;
  }
  return env;
}

Cell* find_sym(Lenv* env, Lval* lval_sym) {
  while (env->kv) {
    Lval* lval = env->kv->car;
    if (strcmp(lval->sym, lval_sym->sym) == 0) return env->kv;
    env->kv = env->kv->cdr;
  }
  return NULL;
}

int lenv_is_bound(Lenv* env, Lval* lval_sym) {
  return find_sym(env, lval_sym) ? 1 : 0;
}

// Mutable
Lenv* lenv_put(Lenv* env, Lval* lval_sym, Lval* lval) {
  Cell* cell = find_sym(env, lval_sym);
  if (cell) {
    cell = cell->cdr;
    lval_del(cell->car);
    cell->car = lval;
  } else {
    Cell* old_head = env->kv;
    cell = make_cell();
    env->kv = cell;
    cell->car = lval_sym;
    cell->cdr = make_cell();
    cell->car = lval;
    cell->cdr = old_head;
  }
  return NULL;
}

// Persistent
Lenv* lenv_assoc(Lenv* env, Lval* lval_sym, Lval* lval) {
  Lenv* env2 = lenv_new();
  Cell* cell = NULL;
  env2->parent_env = env->parent_env;
  // V1: just stick it in front of the lookup list
  Cell* old_head = env->kv;
  cell = make_cell();
  env2->kv = cell;
  cell->car = lval_sym;
  cell->cdr = make_cell();
  cell->car = lval;
  cell->cdr = old_head;
  return env2;

  // V2 copy path to cell
  /* cell = find_sym(env, lval_sym); */
  /* if (cell) { */
  /*   Cell* new_head = make_cell(); */
  /*   env2->kv = new_head; */
  /*   Cell* tail = copy_list(env->kv, new_head, cell); */
  /*   Cell* new_sym_cell = make_cell(); */
  /*   tail->cdr = new_sym_cell; */
  /*   new_sym_cell->car = lval_sym; */
  /*   Cell* new_lval_cell = make_cell(); */
  /*   new_sym_cell->cdr = new_lval_cell; */
  /*   new_lval_cell->car = lval; */
  /*   new_lval_cell->cdr = cell->cdr->cdr; */
  /*   return env2; */
  /* } else { */
  /*   Cell* old_head = env->kv; */
  /*   cell = make_cell(); */
  /*   env2->kv = cell; */
  /*   cell->car = lval_sym; */
  /*   cell->cdr = make_cell(); */
  /*   cell->car = lval; */
  /*   cell->cdr = old_head; */
  /*   return env2; */
  /* } */
}

void lenv_add_builtin(Lenv* env, char* name, lbuiltin func, int subtype) {
  Lval* lval_sym = make_lval_sym(name);
  if (lenv_is_bound(get_root_env(env), lval_sym)) {
    printf("Warning: duplicate builtin fn: '%s'\n", lval_sym->sym);
  }
  Lval* lval_fun = make_lval_fun(func, name, subtype);
  lenv_put(env, lval_sym, lval_fun);
}
