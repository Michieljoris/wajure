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

    mpc_result_t result;
    if (mpc_parse("<stdin>", input, Lispy, &result)) {
      mpc_ast_t* t = result.output;
      Lval* expressions =
          read_expressions(make_lval_sexpr(), t->children, t->children_num);
      mpc_ast_delete(result.output);
      while (expressions->count) {
        Lval* expr = lval_pop(expressions, 0);
        /* Lval* x = lval_eval(env, expr); */
        /* lval_println(x); */
        /* lval_del(x); */
        lval_println(expr);
        lval_del(expr);
      }
      lval_del(expressions);

    } else {
      printf("Error\n");
      mpc_err_print(result.error);
      mpc_err_print(result.error);
    }

    /* Add input to history */
    add_history(input);
    free(input);
  }
}
