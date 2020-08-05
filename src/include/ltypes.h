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
typedef struct wval Wval;

typedef Lval* (*Lbuiltin)(Lenv*, Lval*);

typedef struct {
  char* lispy_fn_name;
  Lbuiltin fun;
  char* c_fn_name;
  int params_count;
  int results_count;
} LispyFn;

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

typedef struct context Context;

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

  // compiler to wasm data
  Context* context;
  int offset;
};

struct wval {
  char type;
  char subtype;

  short fn_table_index;
  short param_count;
  short has_rest_arg;
  short partial_count;
  Lval** closure;
  Lval** partials;
};

struct lenv {
  int is_user_env;
  Lenv* parent_env;
  Cell* kv;
};

// Compile structs
typedef struct {
  int local_count;
  char* fn_name;
  int closure_count;
  Lenv* closure;
} FunctionContext;

struct context {
  char* msg;
  Lval* lval;
  Cell* cell;
  FunctionContext* function_context;
  // whatever
  int line;
  int pos;
};

/* lval types */
enum {
  LVAL_SYMBOL,
  LVAL_COLLECTION,
  LVAL_LITERAL,
  LVAL_FUNCTION,
  LVAL_ERR,
  /* subtypes */
  // literal subtypes
  LNIL,
  LTRUE,
  LFALSE,
  NUMBER,
  STRING,
  REGEX,
  KEYWORD,
  // function subtypes
  SYS,
  MACRO,
  SPECIAL,
  LAMBDA,
  // collection subtypes
  LIST,
  MAP,
  VECTOR,
  SET,
  // error subtypes
  USER,
  // compiler types
  LVAL_WASM_REF,
  // local ref subtypes
  PARAM,
  LOCAL,

  // lispy runtime lval fn type
  LVAL_WASM_LAMBDA
};

#endif  // __LTYPES_H_
