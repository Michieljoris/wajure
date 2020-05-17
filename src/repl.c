#include <editline/history.h>
#include <editline/readline.h>
#include <stdio.h>

#include "env.h"
#include "eval.h"
#include "fns.h"
#include "grammar.h"
#include "lval.h"
#include "mpc.h"
#include "print.h"
#include "read.h"

void repl() {
  /* puts("Press Ctrl+c to Exit\n"); */
  mpc_parser_t* Lispy = init_grammar();
  Lenv* e = lenv_new();
  lenv_add_builtins(e);

  while (!exit_repl) {
    /* Output our prompt and get input*/
    char* input = readline(">");

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      /* printf("Success\n"); */
      Lval* ast = lval_read(r.output);
      lval_println(ast);
      Lval* result = lval_eval(e, ast);
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
  lenv_del(e);
  /* Undefine and Delete our Parsers */
  grammar_cleanup();
}
