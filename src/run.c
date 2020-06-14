#include "run.h"

#include "debug.h"
#include "env.h"
#include "fns.h"
#include "io.h"
#include "iter.h"
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

  // Add builtins to root  env
  Lenv* root_env = lenv_new();
  lenv_add_builtin_fns(root_env);

  /* Lenv* user_env = lenv_new(); */
  /* user_env->parent_env = retain(root_env); */

  // DEBUG
  set_debug_level(1);
  print_mempool_counts();
  printf(" after adding builtin fns\n");

  // Read in list of (string) arguments from commandline

  // For now we only understand args to be a list of file names to be read in
  for (int i = 1; i < argc; ++i) {
    printf("Slurping %s\n", argv[i]);
    Lval* result = slurp(root_env, argv[i]);

    printf("\n\n------------> Result of slurping %s: ", argv[i]);
    lval_println(result);
    printf("ref count of result: %d\n", get_ref_count(result));
    printf("releasing result:") release(result);
    printf("\n");
    print_mempool_counts();
    printf("after releasing result of slurp of %s ", argv[i]);
  }
  printf("\n");
  /* print_mempool_counts(); */
  /* printf("after slurping\n"); */
  set_debug_level(0);
  release(root_env->kv);
  release(root_env);
  set_debug_level(1);
  print_mempool_counts();
  printf(" after releasing root_env\n");
  free_lispy_mempools();
}
