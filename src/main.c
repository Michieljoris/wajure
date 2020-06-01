#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "env.h"
#include "fns.h"
#include "grammar.h"
#include "lval.h"
#include "memory_pool.h"
#include "misc_fns.h"
#include "print.h"
#include "repl.h"

void test_memory_pool() {
  MemoryPool* mp = create_memory_pool(4, 2, MP_AUTO_RESIZE);

  int* d1 = memory_pool_alloc(mp);
  *d1 = 111;
  printf("%d\n", *d1);

  int* d2 = memory_pool_alloc(mp);
  *d2 = 222;
  printf("%d\n", *d2);

  memory_pool_debug(mp);
  /* memory_pool_free(mp, d2); */

  /* memory_pool_debug(mp); */

  int* d3 = memory_pool_alloc(mp);
  *d3 = 333;
  printf("%d\n", *d3);

  printf("Done");
}

int main(int argc, char** argv) {
  test_memory_pool();
  return 0;

  set_debug_level(1);
  init_grammar();
  Lenv* root_env = lenv_new();
  lenv_add_sys_fns(root_env);
  Lenv* user_env = lenv_new();
  user_env->parent_env = root_env;
  if (argc >= 2) {
    for (int i = 1; i < argc; ++i) {
      Lval* args = lval_add_child(make_lval_sexpr(), make_lval_str(argv[i]));
      Lval* x = load_fn(user_env, args);
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
  return 0;
}
