#include "run.h"

#include "env.h"
#include "fns.h"
#include "grammar.h"
#include "io.h"
#include "lispy_mempool.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"
#include "repl.h"

void run(int argc, char** argv) {
  init_lispy_mempools(100, 100, 100);
  init_grammar();
  Lenv* root_env = lenv_new();
  lenv_add_sys_fns(root_env);
  Lenv* user_env = lenv_new();
  user_env->parent_env = root_env;
  if (argc >= 2) {
    for (int i = 1; i < argc; ++i) {
      Lval* arg_list = lval_add_child(make_lval_list(), make_lval_str(argv[i]));
      Lval* x = load_fn(user_env, arg_list);
      if (x->type == LVAL_ERR) {
        lval_println(x);
      }
      lval_del(x);
    }
  }
  /* repl(env); */

  lenv_del(user_env);
  lenv_del(root_env);
  grammar_cleanup();
  free_lispy_mempools();
}
