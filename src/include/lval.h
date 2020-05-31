#ifndef __LVAL_H_
#define __LVAL_H_
#include <stdbool.h>

struct lenv;
struct lval;

typedef struct lenv Lenv;

typedef struct lval Lval;

typedef Lval* (*lbuiltin)(Lenv*, Lval*);

struct lval {
  int type;
  int subtype;

  long num; /* TODO: could/should this be a union? */
  char* err;
  char* str;
  char* sym;
  char* func_name;

  /* function */
  lbuiltin fun;
  Lenv* bindings;
  Lval* formals;
  Lval* body;

  /* list of lval */
  int count;
  Lval** node;
  Lenv* tco_env;
};

struct lenv {
  Lenv* parent_env;
  int count;
  char** syms;
  Lval** lvals;
};

struct {
  Lval* lval;
  Lval* cdr;
  int ref_count;
} Cell;

/* lval types */
enum {
  LVAL_NUM,
  LVAL_QUOTE,
  LVAL_ERR,
  LVAL_SYM,
  LVAL_SEQ,
  LVAL_FUN,
  LVAL_STR,
  /* subtypes */
  SYS,
  MACRO,
  SPECIAL,
  LAMBDA,
  LIST,
  MAP,
  VECTOR,
  USER,
  PLIST

};

Lval* make_lval_num(long x);
Lval* make_lval_quote(void);
Lval* make_lval_sym(char* s);
Lval* make_lval_str(char* s);
Lval* make_lval_sexpr(void);
Lval* make_lval_map(void);
Lval* make_lval_vector(void);
Lval* make_lval_fun(lbuiltin func, char* func_name, int type);
Lval* make_lval_lambda(Lenv* env, Lval* formals, Lval* body, int subtype);
Lval* make_lval_macro(Lval* formals, Lval* body);
Lval* make_lval_err(char* fmt, ...);
Lval* make_lval_exception(char* msg);

char* lval_type_to_name(int t);
char* lval_type_to_name2(Lval* lval);

Lval* lval_add_child(Lval* v, Lval* x);
void lval_del(Lval* v);
Lval* make_lval_copy(Lval* v);
Lval* lval_pop(Lval* v, int i);
Lval* lval_take(Lval* v, int i);

Lval* lval_concat(Lval* x, Lval* y);

Lval* make_lval_plist(void);
#endif  // __LVAL_H_
