/* #include <editline/history.h> */
/* #include <editline/readline.h> */

#include "env.h"
#include "eval.h"
#include "io.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"

void repl(Lenv* env) {
  /*  puts("Press Ctrl+c to Exit\n"); */
  /* while (!exit_repl) { */
  /*   /\* Output our prompt and get input*\/ */
  /*   char* input = readline(">"); */

  /*   mpc_result_t result; */
  /*   if (mpc_parse("<stdin>", input, Lispy, &result)) { */
  /*     mpc_ast_t* mpc_ast = result.output; */

  /*     Lval* expressions = read_list(make_lval_list(), mpc_ast); */
  /*     mpc_ast_delete(result.output); */
  /*     while (expressions->count) { */
  /*       // TODO: fix up repl */
  /*       Lval* expr = NULL;  // lval_pop(expressions, 0); */
  /*       Lval* x = lval_eval(env, expr); */
  /*       lval_debugln(x); */
  /*     } */

  /*   } else { */
  /*     printf("Error\n"); */
  /*     mpc_err_print(result.error); */
  /*   } */

  /*   /\* Add input to history *\/ */
  /*   add_history(input); */
  /*   free(input); */
  /* } */
}
