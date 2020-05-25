#include <stdio.h>
#include <stdlib.h>

#include "env.h"
#include "fns.h"
#include "grammar.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"
#include "repl.h"

int main(int argc, char** argv) {
  init_grammar();
  Lenv* env = lenv_new();
  lenv_add_builtins(env);
  if (argc >= 2) {
    for (int i = 1; i < argc; ++i) {
      Lval* args = lval_add_child(make_lval_sexpr(), make_lval_str(argv[i]));
      Lval* x = load_fn(env, args);
      printf("Loaded %s\n", argv[i]);
      if (x->type == LVAL_ERR) {
        lval_println(x);
      }
      lval_del(x);
    }
  }
  /* repl(env); */

  lenv_del(env);
  grammar_cleanup();
  return 0;
}
