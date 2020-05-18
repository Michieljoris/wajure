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

void load_stdlib(Lenv* env, mpc_parser_t* Lispy) {
  char* lines[] = {
      "def {defn} (fn {args body} {def (head args) (fn (tail args) body)})",
      "defn {unpack f xs} {eval (join (list f) xs)}",
      "defn {pack f & xs} {f xs}",
      "def {uncurry} pack",
      "def {curry} unpack",
      "def {apply} unpack"};

  /* defn {reverse f x y} {curry f {y x}}} */
  /* defn {first xs} {curry + (head xs)} */
  /* defn {second xs} {curry + (head (tail xs))} */
  /* (defn {foo f g & xs} {g (apply f xs)}) */

  unsigned long count = sizeof(lines) / sizeof(char*);
  mpc_result_t r;
  Lval* ast;
  for (int i = 0; i < count; ++i) {
    mpc_parse("<stdin>", lines[i], Lispy, &r);
    ast = lval_read(r.output);
    lval_eval(env, ast);
  }
}

void repl() {
  /* puts("Press Ctrl+c to Exit\n"); */
  mpc_parser_t* Lispy = init_grammar();
  Lenv* env = lenv_new();
  lenv_add_builtins(env);
  load_stdlib(env, Lispy);

  while (!exit_repl) {
    /* Output our prompt and get input*/
    char* input = readline(">");

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      /* printf("Success\n"); */
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
  lenv_del(env);
  /* Undefine and Delete our Parsers */
  grammar_cleanup();
}
