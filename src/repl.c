#include <editline/history.h>
#include <editline/readline.h>
#include <stdio.h>

#include "env.h"
#include "eval.h"
#include "grammar.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"
#include "read.h"

void repl(Lenv* env) {
  /*  puts("Press Ctrl+c to Exit\n"); */
  while (!exit_repl) {
    /* Output our prompt and get input*/
    char* input = readline(">");

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      Lval* ast = lval_read(r.output);
      lval_println(ast);
      Lval* result = lval_eval(env, ast);
      lval_println(result);
      lval_del(result);
      mpc_ast_delete(r.output);
    } else {
      printf("Error\n");
      mpc_err_print(r.error);
      mpc_err_print(r.error);
    }

    /* Add input to history */
    add_history(input);
    free(input);
  }
}
