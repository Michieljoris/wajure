#include "env.h"
#include "io.h"
#include "lispy_mempool.h"
#include "list_fns.h"
#include "math_fns.h"
#include "misc_fns.h"
#include "special_fns.h"
#include "util_fns.h"

void lenv_add_builtin(Lenv* env, char* name, Lbuiltin func, int type) {
  char* lname = lalloc_size(_strlen(name));
  _strcpy(lname, name);
  Lval* lval_sym = make_lval_sym(lname);
  if (lenv_is_bound(get_root_env(env), lval_sym)) {
    printf("Warning: duplicate builtin fn: '%s'\n", lval_sym->str);
  }
  Lval* lval_fun = make_lval_fun(func, lname, type);
  release(lname);
  lenv_put(env, lval_sym, lval_fun);
  release(lval_sym);
  release(lval_fun);
}

void lenv_add_builtins(Lenv* env, RuntimeFn builtins[], int type) {
  for (int i = 0; builtins[i].wajure_fn_name; i++) {
    lenv_add_builtin(env, builtins[i].wajure_fn_name, builtins[i].fun, type);
  }
}

extern RuntimeFn misc_builtins[];
extern RuntimeFn list_builtin_fns[];
extern RuntimeFn math_builtin_fns[];
extern RuntimeFn special_builtins[];
extern RuntimeFn util_builtin_fns[];

void lenv_add_builtin_fns(Lenv* env) {
  printf("adding builtins\n");
  lenv_add_builtins(env, math_builtin_fns, SYS);
  lenv_add_builtins(env, list_builtin_fns, SYS);
  lenv_add_builtins(env, misc_builtins, SYS);
  lenv_add_builtins(env, special_builtins, SPECIAL);
  lenv_add_builtins(env, util_builtin_fns, SYS);
}
