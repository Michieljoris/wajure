#ifndef __LVAL_H_
#define __LVAL_H_

#include "list.h"

struct lenv;
struct lval;

typedef struct lenv Lenv;

typedef struct lval Lval;

typedef Lval* (*Lbuiltin)(Lenv*, Lval*);

typedef struct {
  char* func_name;
  Lbuiltin fun;
} Builtin;

struct lval {
  int type;
  int subtype;

  long num; /* TODO: could/should this be a union? */
  char* err;
  char* str;
  char* sym;
  char* func_name;

  /* function */
  Lbuiltin fun;
  Lenv* bindings;
  Lval* params;
  Lval* body;

  /* list of lval */
  int count;
  Lval** node;
  Lenv* tco_env;

  Cell* head;
};

struct lenv {
  Lenv* parent_env;
  Cell* kv;
};

/* lval types */
enum {
  LVAL_SYMBOL,
  LVAL_COLLECTION,
  LVAL_LITERAL,
  LVAL_FUNCTION,
  LVAL_ERR,
  /* subtypes */
  NUMBER,
  STRING,
  BUILTIN,
  MACRO,
  SPECIAL,
  LAMBDA,
  LIST,
  MAP,
  VECTOR,
  USER
};

Lval* make_lval_num(long x);
Lval* make_lval_quote(void);
Lval* make_lval_sym(char* s);
Lval* make_lval_str(char* s);
Lval* make_lval_list(void);
Lval* make_lval_map(void);
Lval* make_lval_vector(void);
Lval* make_lval_fun(Lbuiltin func, char* func_name, int type);
Lval* make_lval_lambda(Lenv* env, Lval* formals, Lval* body, int subtype);
Lval* make_lval_macro(Lval* formals, Lval* body);
Lval* make_lval_err(char* fmt, ...);
Lval* make_lval_exception(char* msg);

char* lval_type_constant_to_name(int t);
char* lval_type_to_name(Lval* lval);

Cell* make_cell();

Cell* iter_new(Lval* lval_list);
Cell* iter_cell(Cell* iterator);
Cell* iter_current_cell(Cell* iterator);
Lval* iter_next(Cell* iterator);
void iter_end(Cell* iterator);
Lval* iter_peek(Cell* iterator);

#endif  // __LVAL_H_
