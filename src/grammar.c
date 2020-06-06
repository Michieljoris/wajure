#include "grammar.h"

#include "mpc.h"

mpc_parser_t* Number;

mpc_parser_t* Quote;
mpc_parser_t* BackQuote;
mpc_parser_t* TildeAt;
mpc_parser_t* Tilde;

mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* List;
mpc_parser_t* Vector;
mpc_parser_t* Map;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;
mpc_parser_t* Plist;

void init_grammar() {
  /* Create Some Parsers */
  Number = mpc_new("number");
  Quote = mpc_new("quote");
  BackQuote = mpc_new("backquote");
  TildeAt = mpc_new("tilde_at");
  Tilde = mpc_new("tilde");
  Symbol = mpc_new("symbol");
  String = mpc_new("string");
  Comment = mpc_new("comment");
  List = mpc_new("list");
  Vector = mpc_new("vector");
  Map = mpc_new("map");
  Expr = mpc_new("expr");
  Lispy = mpc_new("lispy");
  Plist = mpc_new("plist");
  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
            "                                    \
    number  : /-?[0-9]+/ ;                       \
    symbol  : /[<>a-zA-Z0-9_+\\-*\\/\\\\=!&]+/ ; \
    string  : /\"(\\\\.|[^\"])*\"/ ;             \
    quote   : /'/ ;                              \
    backquote   : /`/ ;                          \
    tilde_at : /~@/ ;                             \
    tilde   : /~/ ;                              \
    comment : /;[^\\r\\n]*/ ;                    \
    list   : '(' <expr>* ')' ;                  \
    vector   : '[' <expr>* ']' ;                 \
    map     : '{' <expr>* '}' ;                  \
    expr    : <number>  | <quote> | <symbol> | <string> | <tilde_at>    \
            | <comment> | <list>  | <vector> | <backquote> | <tilde> | <plist> ;    \
    lispy   : /^/ <expr>* /$/ ;                  \
  ",
            Number, Quote, BackQuote, Tilde, TildeAt, Symbol, List, Vector,
            String, Comment, Map, Expr, Lispy, Plist);
}

void grammar_cleanup() {
  /* Undefine and Delete our Parsers */
  mpc_cleanup(14, Number, Quote, BackQuote, Tilde, TildeAt, Symbol, String,
              Comment, List, Vector, Expr, Map, Lispy, Plist);
}
