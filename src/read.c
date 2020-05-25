#include <stdlib.h>

#include "lval.h"
#include "mpc.h"

typedef struct {
  mpc_ast_t** expressions;
  int count;
  int index;
} Expr_stream;

mpc_ast_t* get_next_expr(Expr_stream* expr_stream) {
  return expr_stream->expressions[expr_stream->index++];
}

bool has_next_expr(Expr_stream* expr_stream) {
  return expr_stream->index < expr_stream->count;
}

static Lval* lval_read_str(char* token) {
  /* remove quotes either side */
  token[strlen(token) - 1] = '\0';
  char* unescaped = malloc(strlen(token + 1) + 1);
  strcpy(unescaped, token + 1);
  unescaped = mpcf_unescape(unescaped);
  Lval* str = make_lval_str(unescaped);
  free(unescaped);
  return str;
}

static Lval* lval_read_num(char* token) {
  errno = 0;
  long x = strtol(token, NULL, 10);
  return errno != ERANGE ? make_lval_num(x) : make_lval_err("invalid number");
}

Lval* read_next_expression(Expr_stream* expr_stream);

static Lval* lval_read_quote(Expr_stream* expr_stream) {
  Lval* next_expression = read_next_expression(expr_stream);
  if (!next_expression) {
    return make_lval_err("The reader macro \"'\" has nothing to quote.");
  }
  Lval* lval = make_lval_sexpr();
  lval = lval_add_child(lval, make_lval_sym("quote"));
  lval = lval_add_child(lval, next_expression);
  return lval;
}

enum {
  NUMBER_TOKEN,
  SYMBOL_TOKEN,
  QUOTE_TOKEN,
  STRING_TOKEN,
  SEXPR,
  QEXPR,
  REGEX,
  COMMENT,
  IGNORE,
  ERROR
};

char* get_expr_token(mpc_ast_t* expr) { return expr->contents; }

int get_expr_type(mpc_ast_t* expr, char* token) {
  char* tag = expr->tag;
  if (strstr(tag, "number")) {
    return NUMBER_TOKEN;
  }
  if (strstr(tag, "symbol")) {
    return SYMBOL_TOKEN;
  }
  if (strstr(tag, "quote")) {
    return QUOTE_TOKEN;
  }
  if (strstr(tag, "string")) {
    return STRING_TOKEN;
  }
  if (strstr(tag, "sexpr")) {
    return SEXPR;
  }
  if (strstr(tag, "qexpr")) {
    return QEXPR;
  }
  if (strcmp(tag, "regex") == 0 || strstr(tag, "comment") ||
      strcmp(token, "(") == 0 || strcmp(token, ")") == 0 ||
      strcmp(token, "{") == 0 || strcmp(token, "}") == 0)
    return IGNORE;
  return ERROR;
}

Lval* read_expressions(Lval* lval, mpc_ast_t* expressions[],
                       int expression_count);

Lval* read_next_expression(Expr_stream* expr_stream) {
  mpc_ast_t* expression = get_next_expr(expr_stream);
  char* token = get_expr_token(expression);
  /* printf("token %s %s\n", token, expression->tag); */
  int expr_type = get_expr_type(expression, token);
  Lval* lval_from_expr;
  switch (expr_type) {
    case IGNORE:
      return NULL;
    case NUMBER_TOKEN:
      lval_from_expr = lval_read_num(token);
      break;
    case SYMBOL_TOKEN:
      lval_from_expr = make_lval_sym(token);
      break;
    case STRING_TOKEN:
      lval_from_expr = lval_read_str(token);
      break;
    case QUOTE_TOKEN:
      lval_from_expr = lval_read_quote(expr_stream);
      break;
    case QEXPR:
      lval_from_expr = read_expressions(make_lval_qexpr(), expression->children,
                                        expression->children_num);
      break;
    case SEXPR:
      lval_from_expr = read_expressions(make_lval_sexpr(), expression->children,
                                        expression->children_num);
      break;
    case ERROR:
      lval_from_expr = make_lval_err("unknown token %s", token);
  }
  return lval_from_expr;
}

Lval* read_expressions(Lval* lval, mpc_ast_t* expressions[],
                       int expression_count) {
  /* printf("read_expresions %d\n", expression_count); */
  Expr_stream expr_stream = {expressions, expression_count, 0};
  while (has_next_expr(&expr_stream)) {
    Lval* next_expr = read_next_expression(&expr_stream);
    if (next_expr) lval = lval_add_child(lval, next_expr);
  }
  return lval;
}

/* Lval* lval_read(mpc_ast_t* t) { */
/*   printf("reading: %s\n", t->contents); */
/*   if (strstr(t->tag, "number")) { */
/*     return lval_read_num(t->contents); */
/*   } */
/*   if (strstr(t->tag, "symbol")) { */
/*     return make_lval_sym(t->contents); */
/*   } */
/*   if (strstr(t->tag, "quote")) { */
/*     return make_lval_quote(); */
/*   } */
/*   if (strstr(t->tag, "string")) { */
/*     return lval_read_str(t->contents); */
/*   } */

/*   Lval* lval_sexpr = NULL; */
/*   if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "sexpr")) { */
/*     lval_sexpr = make_lval_sexpr(); */
/*   } */
/*   if (strstr(t->tag, "qexpr")) { */
/*     lval_sexpr = make_lval_qexpr(); */
/*   } */
/*   for (int i = 0; i < t->children_num; i++) { */
/*     char* contents = t->children[i]->contents; */
/*     if (strcmp(contents, "(") == 0 || strcmp(contents, ")") == 0 || */
/*         strcmp(contents, "{") == 0 || strcmp(contents, "}") == 0 || */
/*         strcmp(t->children[i]->tag, "regex") == 0 || */
/*         strstr(t->children[i]->tag, "comment")) { */
/*       continue; */
/*     } */
/*     lval_sexpr = lval_add_child(lval_sexpr, lval_read(t->children[i])); */
/*   } */
/*   return lval_sexpr; */
/* } */
