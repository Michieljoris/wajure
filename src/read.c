#include <stdlib.h>

#include "lval.h"
#include "mpc.h"

static Lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ? make_lval_num(x) : make_lval_err("invalid number");
}

Lval* lval_read(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) {
    return lval_read_num(t);
  }
  if (strstr(t->tag, "symbol")) {
    return make_lval_sym(t->contents);
  }
  Lval* x = NULL;
  if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "sexpr")) {
    x = make_lval_sexpr();
  }
  if (strstr(t->tag, "qexpr")) {
    x = make_lval_qexpr();
  }
  for (int i = 0; i < t->children_num; i++) {
    char* contents = t->children[i]->contents;
    if (strcmp(contents, "(") == 0 || strcmp(contents, ")") == 0 ||
        strcmp(contents, "{") == 0 || strcmp(contents, "}") == 0 ||
        strcmp(t->children[i]->tag, "regex") == 0) {
      continue;
    }
    x = lval_add_child(x, lval_read(t->children[i]));
  }
  return x;
}
