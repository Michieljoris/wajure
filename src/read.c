#include "lib.h"
#include "lval.h"
#include "mpc.h"
#include "plist_fns.h"
#include "print.h"

typedef struct {
  mpc_ast_t** expressions;
  int count;
  int index;
} Expr_stream;

int do_ignore(mpc_ast_t* expression) {
  char* token = expression->contents;
  char* tag = expression->tag;
  /* printf("token %s\n", token); */
  /* printf("tag %s\n", tag); */
  return (_strcmp(tag, "regex") == 0 || _strstr(tag, "comment") ||
          _strcmp(token, "(") == 0 || _strcmp(token, ")") == 0 ||
          _strcmp(token, "[") == 0 || _strcmp(token, "]") == 0 ||
          _strcmp(token, "{") == 0 || _strcmp(token, "}") == 0 ||
          _strcmp(token, "<") == 0 || _strcmp(token, ">") == 0);
}

mpc_ast_t* get_next_expr(Expr_stream* expr_stream) {
  return expr_stream->expressions[expr_stream->index++];
}

int has_next_expr(Expr_stream* expr_stream) {
  mpc_ast_t* expression = NULL;
  while (expr_stream->index < expr_stream->count) {
    expression = expr_stream->expressions[expr_stream->index];
    if (do_ignore(expression)) {
      expr_stream->index++;
    } else {
      break;
    }
  }
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

#define READER_MACRO(fn, reader_token, lispy_fn)                            \
  static Lval* fn(Expr_stream* expr_stream) {                               \
    Lval* next_expression = read_next_expression(expr_stream);              \
    if (!next_expression)                                                   \
      return make_lval_err("The reader macro \"%s\" has nothing to quote.", \
                           reader_token);                                   \
    Lval* lval = make_lval_list();                                          \
    lval = lval_add_child(lval, make_lval_sym(lispy_fn));                   \
    lval = lval_add_child(lval, next_expression);                           \
    return lval;                                                            \
  }

READER_MACRO(lval_read_quote, "'", "quote");
READER_MACRO(lval_read_backquote, "`", "quasiquote");
READER_MACRO(lval_read_tilde, "~", "unquote");
READER_MACRO(lval_read_tilde_at, "~@", "splice-unquote");

Lval* lval_read_plist(Lval* plist, mpc_ast_t* expression);

Lval* read_expressions(Lval* lval, mpc_ast_t* expressions[],
                       int expression_count);

Lval* read_next_expression(Expr_stream* expr_stream) {
  mpc_ast_t* expression = get_next_expr(expr_stream);
  char* token = expression->contents;
  char* tag = expression->tag;
  /* printf("token %s TAG: %s\n", token, expression->tag); */
  /* printf("tag: %s\n", tag); */
  if (_strstr(tag, "number")) return lval_read_num(token);
  if (_strstr(tag, "symbol")) return make_lval_sym(token);
  if (_strstr(tag, "|quote|")) return lval_read_quote(expr_stream);
  if (_strstr(tag, "backquote")) return lval_read_backquote(expr_stream);
  if (_strstr(tag, "|tilde|")) return lval_read_tilde(expr_stream);
  if (_strstr(tag, "tilde_at")) return lval_read_tilde_at(expr_stream);
  if (_strstr(tag, "string")) return lval_read_str(token);
  if (_strstr(tag, "plist")) {
    return lval_read_plist(make_lval_plist(), expression);
  }
  if (_strstr(tag, "list"))
    return read_expressions(make_lval_list(), expression->children,
                            expression->children_num);
  if (_strstr(tag, "map"))
    return read_expressions(make_lval_map(), expression->children,
                            expression->children_num);
  if (_strstr(tag, "vector"))
    return read_expressions(make_lval_vector(), expression->children,
                            expression->children_num);
  return make_lval_err("unknown token %s", token);
}

Lval* lval_read_plist(Lval* plist, mpc_ast_t* expression) {
  Expr_stream expr_stream = {expression->children, expression->children_num, 0};
  Lval* next_expr = NULL;
  Cell* cell = NULL;
  if (has_next_expr(&expr_stream)) {
    next_expr = read_next_expression(&expr_stream);
    plist->cell = cell = make_cell();
    cell->car = next_expr;
    while (has_next_expr(&expr_stream)) {
      Lval* next_expr = read_next_expression(&expr_stream);
      Cell* next_cell = make_cell();
      cell->cdr = next_cell;
      next_cell->car = next_expr;
      cell = next_cell;
    }
  }
  /* lval_println(plist); */

  return plist;
}

Lval* read_expressions(Lval* lval, mpc_ast_t* expressions[],
                       int expression_count) {
  /* printf("read_expresions %d\n", expression_count); */
  Expr_stream expr_stream = {expressions, expression_count, 0};
  while (has_next_expr(&expr_stream)) {
    Lval* next_expr = read_next_expression(&expr_stream);
    lval = lval_add_child(lval, next_expr);
  }
  return lval;
}
