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
  printf("\n");
  set_debug_level(0);
  Lenv* root_env = lenv_new();
  lenv_add_builtin_fns(root_env);  // add builtins
  /* Lenv* user_env = lenv_new(); */
  /* user_env->parent_env = retain(root_env); */
  set_debug_level(1);
  print_mempool_free_all();
  printf(" after adding builtin fns\n");

  if (argc >= 2) {
    for (int i = 1; i < argc; ++i) {
      Lval* arg_list = make_lval_list();
      arg_list->head = make_cell();
      arg_list->head->car = make_lval_str(argv[i]);
      printf("done making arglist for load_fn\n");
      Lval* x = load_fn(root_env, arg_list);  // load_fn
      printf("done load_fn, result: ");
      lval_println(x);
      release(x);
      printf("done releasing result\n");
      /* debug("foo", arg_list); */

      release(arg_list);

      printf("done releasing arg_list\n");

      /* if (x->type == LVAL_ERR) { */
      /*   lval_println(x); */
      /* } */
    }
  }
  /* repl(env); */
  /* set_debug_level(0); */
  /* release(user_env); */
  /* set_debug_level(1); */
  /* print_mempool_free_all(); */
  /* printf(" after releasing user_env\n"); */
  set_debug_level(1);
  /* printf("refcount of root env: %d\n", get_ref_count(root_env)); */
  print_mempool_free_all();
  set_debug_level(0);
  release(root_env);
  set_debug_level(1);
  print_mempool_free_all();
  printf(" after releasing root_env\n");
  free_lispy_mempools();
}
