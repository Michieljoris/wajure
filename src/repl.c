#include "eval.h"
#include "grammar.h"
#include "lval.h"
#include "print.h"
#include "read.h"

#include "mpc.h"
#include <editline/history.h>
#include <editline/readline.h>
#include <stdio.h>

void repl()
{
    puts("Press Ctrl+c to Exit\n");
    mpc_parser_t* Lispy = init_grammar();

    while (1) {
        /* Output our prompt and get input*/
        char* input = readline("lispy>");

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            /* printf("Success\n"); */
            lval* ast = lval_read(r.output);
            lval* result = lval_eval(ast);
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
    /* Undefine and Delete our Parsers */
    grammar_cleanup();
}
