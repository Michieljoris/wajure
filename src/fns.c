#include "env.h"
#include "io.h"
#include "lispy_mempool.h"
#include "list_fns.h"
#include "math_fns.h"
#include "misc_fns.h"
#include "special_fns.h"

void lenv_add_builtin(Lenv* env, char* name, Lbuiltin func, int type) {
  char* lname = lalloc_size(_strlen(name));
  _strcpy(lname, name);
  Lval* lval_sym = make_lval_sym(lname);
  if (lenv_is_bound(get_root_env(env), lval_sym)) {
    printf("Warning: duplicate builtin fn: '%s'\n", lval_sym->sym);
  }
  Lval* lval_fun = make_lval_fun(func, lname, type);
  release(lname);
  lenv_put(env, lval_sym, lval_fun);
}

void lenv_add_builtins(Lenv* env, Builtin builtins[], int type) {
  for (int i = 0; builtins[i].func_name; i++) {
    lenv_add_builtin(env, builtins[i].func_name, builtins[i].fun, type);
  }
}

extern Builtin misc_builtins[];
extern Builtin list_builtins[];
extern Builtin math_builtins[];
extern Builtin special_builtins[];

void lenv_add_builtin_fns(Lenv* env) {
  printf("adding builtins\n");
  lenv_add_builtins(env, math_builtins, SYS);
  lenv_add_builtins(env, list_builtins, SYS);
  lenv_add_builtins(env, misc_builtins, SYS);
  lenv_add_builtins(env, special_builtins, SPECIAL);
}
