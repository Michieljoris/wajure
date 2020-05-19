#include "grammar.h"

#include "mpc.h"

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;

void init_grammar() {
  /* Create Some Parsers */
  Number = mpc_new("number");
  Symbol = mpc_new("symbol");
  String = mpc_new("string");
  Comment = mpc_new("comment");
  Sexpr = mpc_new("sexpr");
  Qexpr = mpc_new("qexpr");
  Expr = mpc_new("expr");
  Lispy = mpc_new("lispy");
  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
            "                                              \
    number  : /-?[0-9]+/ ;                       \
    symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; \
    string  : /\"(\\\\.|[^\"])*\"/ ;             \
    comment : /;[^\\r\\n]*/ ;                    \
    sexpr   : '(' <expr>* ')' ;                  \
    qexpr   : '{' <expr>* '}' ;                  \
    expr    : <number>  | <symbol> | <string>    \
            | <comment> | <sexpr>  | <qexpr>;    \
    lispy   : /^/ <expr>* /$/ ;                  \
  ",
            Number, Symbol, Sexpr, String, Comment, Qexpr, Expr, Lispy);
}

void grammar_cleanup() {
  /* Undefine and Delete our Parsers */
  mpc_cleanup(8, Number, Symbol, String, Comment, Sexpr, Expr, Qexpr, Lispy);
}
