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

/* struct fun { */
/*   Lbuiltin fun; */
/*   Lenv* closure; */
/*   Lval* params; */
/*   Lval* body; */
/* }; */

/* struct lval2 { */
/*   int type; */
/*   int subtype; */
/*   void* pointer;  // num, str, fun or cell */
/*   int hash; */
/* }; */

struct lval {
  char type;
  char subtype;

  // Number, error, symbol or string
  long num;
  char* str; /* Function */

  // List
  Cell* head;
  int hash;

  Lbuiltin fun;
  Lenv* closure;
  Lval* params;
  Lval* body;

  int offset;  // compiler to wasm data
};

struct lenv {
  int is_user_env;
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
