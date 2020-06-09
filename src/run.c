#include "run.h"

#include "env.h"
#include "fns.h"
#include "io.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"
#include "repl.h"

void clean_up(Lval** avar) {
  printf("Cleaning up\n");
  lval_println(*avar);
  /* printf("Final value: %d\n",*avar); */
}

void fox(int x) { printf("%d\n", x); }
void bax(int x) { printf("%d\n", x); }

typedef void (*Foo)(int);
Foo fn[] = {fox, bax};

void run(int argc, char** argv) {
  init_lispy_mempools(100, 100, 100);
  Lval* list = make_lval_list();
  printf("type: %p\n", list->head);
  fn[0](123);
  return;

  Lval* avar __attribute__((__cleanup__(clean_up))) = make_lval_num(123);
  /* avar = 123; */
  /* return; */
  avar = make_lval_num(456);
  /* avar = 345; */
  Lenv* root_env = lenv_new();
  /* lenv_add_sys_fns(root_env); */
  Lenv* user_env = lenv_new();
  user_env->parent_env = root_env;
  if (argc >= 2) {
    for (int i = 1; i < argc; ++i) {
      printf("make arg_list:");
      Lval* arg_list = make_lval_list();
      printf("make arg_list cell:");
      arg_list->head = make_cell();
      printf("make str lval:");
      arg_list->head->car = make_lval_str(argv[i]);
      Lval* x = load_fn(user_env, arg_list);
      printf("done\n");
      lfree(LVAL, arg_list);
      if (x->type == LVAL_ERR) {
        lval_println(x);
      }
      lval_del(x);
    }
  }
  /* repl(env); */

  lenv_del(user_env);
  lenv_del(root_env);
  free_lispy_mempools();
}
