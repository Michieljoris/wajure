#include "run.h"

#include "env.h"
#include "fns.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "misc_fns.h"
#include "namespace.h"
#include "print.h"
#include "repl.h"
#include "state.h"

void run() {
  set_log_level(LOG_LEVEL_INFO);
  install_builtins();
  Namespace* user_ns = install_ns(config->user);
  set_current_ns(user_ns);
  install_stdlib();

  printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  Lval* result = require_ns(NULL, config->main);
  printf("Result of loading %s: ", config->main);
  lval_println(result);
  release(result);
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++");

  print_mempool_counts();
  release(state->builtins_env);
  release(state->namespaces);
  print_mempool_counts();
  printf("\n");
}

/* print_mempool_counts(); */
/* printf(" after adding builtin fns\n"); */

// Add builtins to root  env
/* stdlib_env->is_ns_env = 1; */
/* Lval* ret = load(stdlib_env, "clj/wajure/core.clj"); */
/* stdlib_env->is_ns_env = 0; */
/* lval_println(ret); */
/* release(ret); */

/* SlotCount after_builtins = get_slot_count(); */

/* Lenv* refer_env = lenv_new(); */
/* refer_env->parent_env = retain(stdlib_env); */
/* refer_env->parent_env = retain(root_env); */

/* Lenv* ns_env = lenv_new(); */
/* ns_env->parent_env = retain(stdlib_env); */
/* ns_env->is_ns_env = 1; */

/* set_log_level(LOG_LEVEL_DEEP_DEBUG); */
// DEBUG

// Read in list of (string) arguments from commandline

/* set_log_level(LOG_LEVEL_INFO); */
// For now we only understand args to be a list of file names to be read in
/* char* file_name = ns_to_file_name(config->main); */
/* info("Loading %s\n", file_name); */
/* set_current_ns(user_ns); */
/* Lval* result = require_ns(NULL, config->main); */
/* release(file_name); */

/* printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n"); */
/* printf("Result of loading %s: ", config->main); */
/* lval_println(result); */
/* printf("++++++++++++++++++++++++++++++++++++++++++++++++++"); */
/* /\* printf("ref count of result: %d\n", get_ref_count(result)); *\/ */
/* print_mempool_counts(); */
/* printf("releasing result:"); */
/* release(result); */
/* print_mempool_counts(); */
/* printf("after releasing result of slurp of %s ", argv[i]); */

/* SlotCount after_slurp = get_slot_count(); */
/* printf( */
/*     "\nextra: LENV:%d, LVAL:%d, CELL:%d, ITER:%d after slurp and releasing
 * " */
/*     "result\n", */
/*     after_slurp.env - after_builtins.env, */
/*     after_slurp.lval - after_builtins.lval, */
/*     after_slurp.cell - after_builtins.cell, */
/*     after_slurp.iter - after_builtins.iter); */

/* set_log_level(LOG_LEVEL_INFO); */
/* printf("USER_ENV:\n"); */
/* env_print(user_env); */
/* print_mempool_counts(); */
/* printf("after slurping\n"); */
/* printf("\n-------------- Now going to release user_env!!!!\n"); */
/* release(ns_env->kv); */
/* ns_env->kv = NIL; */
/* release(ns_env); */

/* release(refer_env->kv); */
/* refer_env->kv = NIL; */
/* release(refer_env); */

/* release(stdlib_env->kv); */
/* stdlib_env->kv = NIL; */
/* release(stdlib_env); */
/* print_mempool_counts(); */
/* printf(" after releasing user_env\n"); */
/* SlotCount after_user_env = get_slot_count(); */
/* printf( */
/*     "\nextra: LENV:%d, LVAL:%d, CELL:%d, ITER:%d after releasing
 * user_env\n", */
/*     after_user_env.env - after_builtins.env, */
/*     after_user_env.lval - after_builtins.lval, */
/*     after_user_env.cell - after_builtins.cell, */
/*     after_user_env.iter - after_builtins.iter); */
/* set_log_level(LOG_LEVEL_INFO); */
// TODO:
/* release(root_env); */

/* release(state->namespaces); */
