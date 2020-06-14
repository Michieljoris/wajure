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

struct slot_count {
  int env;
  int lval;
  int cell;
  int iter;
};

struct slot_count get_slot_count() {
  struct slot_count ret = {.env = get_taken_slot_count(LENV),
                           .lval = get_taken_slot_count(LVAL),
                           .cell = get_taken_slot_count(CELL),
                           .iter = get_taken_slot_count(ITER)};
  return ret;
}

void run(int argc, char** argv) {
  init_lispy_mempools(100, 100, 100);
  printf("\n");
  set_debug_level(0);

  // Add builtins to root  env
  Lenv* root_env = lenv_new();
  lenv_add_builtin_fns(root_env);

  struct slot_count after_builtins = get_slot_count();

  Lenv* user_env = lenv_new();
  user_env->parent_env = retain(root_env);

  set_debug_level(1);
  // DEBUG
  print_mempool_counts();
  printf(" after adding builtin fns\n");

  // Read in list of (string) arguments from commandline

  // For now we only understand args to be a list of file names to be read in
  for (int i = 1; i < argc; ++i) {
    printf("Slurping %s\n", argv[i]);
    Lval* result = slurp(user_env, argv[i]);

    printf("\n\n------------> Result of slurping %s: ", argv[i]);
    lval_println(result);
    printf("ref count of result: %d\n", get_ref_count(result));
    printf("releasing result:");
    release(result);
    printf("\n");
    /* print_mempool_counts(); */
    /* printf("after releasing result of slurp of %s ", argv[i]); */
  }

  struct slot_count after_slurp = get_slot_count();
  printf("\nextra: LENV:%d, LVAL:%d, CELL:%d, ITER:%d after slurp\n",
         after_slurp.env - after_builtins.env,
         after_slurp.lval - after_builtins.lval,
         after_slurp.cell - after_builtins.cell,
         after_slurp.iter - after_builtins.iter);
  /* print_mempool_counts(); */
  /* printf("after slurping\n"); */
  set_debug_level(1);
  printf("\n-------------- Now going to release user_env->kv!!!!\n");
  release(user_env->kv);
  printf("\n-------------- Now going to release user_env!!!!\n");
  user_env->kv = NIL;
  release(user_env);
  set_debug_level(1);
  print_mempool_counts();
  printf(" after releasing user_env\n");
  struct slot_count after_user_env = get_slot_count();
  printf(
      "\nextra: LENV:%d, LVAL:%d, CELL:%d, ITER:%d after releasing user_env\n",
      after_user_env.env - after_builtins.env,
      after_user_env.lval - after_builtins.lval,
      after_user_env.cell - after_builtins.cell,
      after_user_env.iter - after_builtins.iter);
  set_debug_level(0);
  release(root_env);
  set_debug_level(1);
  print_mempool_counts();
  printf(" after releasing root_env\n");
  free_lispy_mempools();
}
