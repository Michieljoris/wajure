#include "grammar.h"
#include "mpc.h"

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* Sexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;

mpc_parser_t* init_grammar()
{
    /* Create Some Parsers */
    Number = mpc_new("number");
    Symbol = mpc_new("symbol");
    Sexpr = mpc_new("sexpr");
    Expr = mpc_new("expr");
    Lispy = mpc_new("lispy");
    /* Define them with the following Language */
    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                     \
    number   : /-?[0-9]+/ ;                                             \
    symbol   : '+' | '-' | '*' | '/' ;                                  \
    sexpr    : '(' <expr>* ')' ;                                        \
    expr     : <number> | <symbol> | <sexpr> ;                          \
    lispy    : /^/ <expr>* /$/ ;                                        \
  ",
        Number,
        Symbol,
        Sexpr,
        Expr,
        Lispy);
    return Lispy;
}

void grammar_cleanup()
{
    /* Undefine and Delete our Parsers */
    mpc_cleanup(4, Number, Symbol, Sexpr, Expr, Lispy);
}
