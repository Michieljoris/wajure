#ifndef __LVAL_H_
#define __LVAL_H_

typedef struct lval {
  int type;
  char* sym;
  long num;
  char* err;
  /* list of lval */
  int count;
  struct lval** cell;
} lval;

/* lval types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

lval* lval_add(lval* v, lval* x);
void lval_del(lval* v);
lval* lval_err(char* m);

#endif  // __LVAL_H_
