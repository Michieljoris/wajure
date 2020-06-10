#include "run.h"

#include "debug.h"
#include "env.h"
#include "fns.h"
#include "io.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"
#include "repl.h"

void run(int argc, char** argv) {
  init_lispy_mempools(100, 100, 100);
  Lenv* root_env = lenv_new();
  lenv_add_builtin_fns(root_env);
  Lenv* user_env = lenv_new();
  user_env->parent_env = root_env;
  if (argc >= 2) {
    for (int i = 1; i < argc; ++i) {
      printf("make arg_list:\n");
      Lval* arg_list = make_lval_list();

      printf("make arg_list cell:\n");
      arg_list->head = make_cell();
      printf("make str lval:\n");
      arg_list->head->car = make_lval_str(argv[i]);
      Lval* x = load_fn(user_env, arg_list);
      printf("done\n");
      /* debug("foo", arg_list); */
      release(arg_list);
      if (x->type == LVAL_ERR) {
        lval_println(x);
      }
      release(x);
    }
  }
  /* repl(env); */

  release(user_env);
  release(root_env);
  free_lispy_mempools();
}
