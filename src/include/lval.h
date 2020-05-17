#ifndef __LVAL_H_
#define __LVAL_H_
#include <stdbool.h>

struct lenv;
struct lval;

typedef struct lenv lenv;

typedef struct lval lval;

typedef lval* (*lbuiltin)(lenv*, lval*);

struct lval {
  int type;
  char* sym;
  long num;
  char* err;

  lbuiltin fun;
  char* func_name;
  lenv* env;
  lval* formals;
  lval* body;
  /* list of lval */
  int count;
  struct lval** cell;
};

struct lenv {
  int count;
  char** syms;
  lval** vals;
};

/* lval types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN };

lval* lval_num(long x);
lval* lval_sym(char* s);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_fun(lbuiltin func, char* func_name);
lval* lval_err(char* fmt, ...);
char* ltype_name(int t);

lval* lval_add(lval* v, lval* x);
void lval_del(lval* v);
lval* lval_copy(lval* v);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);

#endif  // __LVAL_H_
