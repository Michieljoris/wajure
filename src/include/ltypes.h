#ifndef __LTYPES_H_
#define __LTYPES_H_

typedef struct cell Cell;

struct cell {
  void* car;
  Cell* cdr;
  int hash;
};

struct lenv;
struct lval;

typedef struct lenv Lenv;

typedef struct lval Lval;
typedef struct wval_fun WvalFun;

typedef Lval* (*Lbuiltin)(Lenv*, Lval*);

// Used to store data on runtime builtin fns
typedef struct {
  char* lispy_fn_name;
  Lbuiltin fun;
  char* c_fn_name;
  int params_count;
  int results_count;
} LispyFn;

// TODO: refactor!!!
struct fun {
  Lbuiltin fun;
  Lenv* closure;
  Lval* params;
  Lval* body;
};

// TODO: refactor: Better and more succinct lval
struct lval2 {
  char type;     // more like the protocol (collection, function, literal etc)
  char subtype;  // more like the actual type (list, map, vector, lambda, macro
                 // etc)
  // num, str, symbol, keyword or pointer to fun or cell (or persistent map
  // (hamt) or pvector)
  void* data;
  int hash;  // or pointer to compiler data
};

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
  int param_count;
  int rest_arg_index;
};

// Used in wasm runtime. We stuff info on a wasm lambda into a lval
struct wval_fun {
  char type;     // wasm offset: 0
  char subtype;  // 1

  short fn_table_index;  // 2
  short param_count;     // 4
  short has_rest_arg;    // 6
  short partial_count;   // 8
  Lval** closure;        // 12
  Lval** partials;       // 16
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
  int param_count;
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
  LVAL_COMPILER,
  // local ref subtypes
  PARAM,
  LOCAL,

  // lispy runtime lval fn type
  // TODO: we can just use the LAMBDA constant
  LVAL_WASM_LAMBDA
};

// Compiler
enum { TOO_MANY_ARGS, TOO_FEW_ARGS, ARGS_MATCH_PARAMS };

#endif  // __LTYPES_H_
