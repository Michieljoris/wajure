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
  char* sym;
  long num;
  char* err;

  /* function */
  lbuiltin fun;
  char* func_name;
  Lenv* env;
  Lval* formals;
  Lval* body;

  /* list of lval */
  int count;
  Lval** cell;
};

struct lenv {
  int count;
  char** syms;
  Lval** vals;
};

/* lval types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN };

Lval* make_lval_num(long x);
Lval* make_lval_sym(char* s);
Lval* make_lval_sexpr(void);
Lval* make_lval_qexpr(void);
Lval* make_lval_fun(lbuiltin func, char* func_name);
Lval* make_lval_err(char* fmt, ...);
char* lval_type_to_name(int t);

Lval* lval_add_child(Lval* v, Lval* x);
void lval_del(Lval* v);
Lval* make_lval_copy(Lval* v);
Lval* lval_pop(Lval* v, int i);
Lval* lval_take(Lval* v, int i);

#endif  // __LVAL_H_
