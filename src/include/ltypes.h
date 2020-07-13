#ifndef __LTYPES_H_
#define __LTYPES_H_

typedef struct cell Cell;

struct cell {
  int hash;
  void* car;
  Cell* cdr;
};

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

  // Number, error, symbol or string
  long num;
  char* str;
  /* Function */
  Lbuiltin fun;
  Lenv* closure;
  Lval* params;
  Lval* body;

  // List
  Cell* head;

  int hash;
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
  LNIL,
  LTRUE,
  LFALSE,
  NUMBER,
  STRING,
  SYS,
  MACRO,
  SPECIAL,
  LAMBDA,
  LIST,
  MAP,
  VECTOR,
  USER
};

#endif  // __LTYPES_H_
