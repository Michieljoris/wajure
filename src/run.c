#include "run.h"

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

/* void run(int argc, char** argv) { */
void run(char* file_name) {
  set_log_level(LOG_LEVEL_INFO);

  // Add builtins to root  env
  Lenv* root_env = lenv_new();
  lenv_add_builtin_fns(root_env);

  SlotCount after_builtins = get_slot_count();

  Lenv* user_env = lenv_new();
  user_env->parent_env = retain(root_env);
  user_env->is_user_env = 1;

  /* set_log_level(LOG_LEVEL_DEEP_DEBUG); */
  // DEBUG
  print_mempool_counts();
  printf(" after adding builtin fns\n");

  // Read in list of (string) arguments from commandline

  set_log_level(LOG_LEVEL_INFO);
  // For now we only understand args to be a list of file names to be read in
  info("Slurping %s\n", file_name);
  Lval* result = slurp(user_env, file_name);

  printf("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("Result of slurping %s: ", file_name);
  lval_println(result);
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("\n");
  /* printf("ref count of result: %d\n", get_ref_count(result)); */
  print_mempool_counts();
  printf("releasing result:");
  release(result);
  /* print_mempool_counts(); */
  /* printf("after releasing result of slurp of %s ", argv[i]); */

  SlotCount after_slurp = get_slot_count();
  printf(
      "\nextra: LENV:%d, LVAL:%d, CELL:%d, ITER:%d after slurp and releasing "
      "result\n",
      after_slurp.env - after_builtins.env,
      after_slurp.lval - after_builtins.lval,
      after_slurp.cell - after_builtins.cell,
      after_slurp.iter - after_builtins.iter);

  set_log_level(LOG_LEVEL_INFO);
  printf("USER_ENV:\n");
  env_print(user_env);
  /* print_mempool_counts(); */
  /* printf("after slurping\n"); */
  /* printf("\n-------------- Now going to release user_env!!!!\n"); */
  release(user_env->kv);
  user_env->kv = NIL;
  release(user_env);
  print_mempool_counts();
  printf(" after releasing user_env\n");
  SlotCount after_user_env = get_slot_count();
  printf(
      "\nextra: LENV:%d, LVAL:%d, CELL:%d, ITER:%d after releasing user_env\n",
      after_user_env.env - after_builtins.env,
      after_user_env.lval - after_builtins.lval,
      after_user_env.cell - after_builtins.cell,
      after_user_env.iter - after_builtins.iter);
  /* set_log_level(LOG_LEVEL_INFO); */
  release(root_env);
  print_mempool_counts();
  printf(" after releasing root_env\n");
  free_lispy_mempools();
}
