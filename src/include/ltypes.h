#ifndef __LTYPES_H_
#define __LTYPES_H_

typedef struct {
  char* src;   // root dir of src files
  char* main;  // filename of file containing main fn
  char* out;   // dir of compiled wasm files
} Config;

typedef struct cell Cell;

struct cell {
  int hash;
  void* car;
  Cell* cdr;
};

#define Map Cell*

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
  int wval_ptr;
  Context* context;
  int offset;
  int param_count;
  int rest_arg_index;
  char* namespace;
};

// Used in wasm runtime. We stuff info on a wasm lambda into a lval
struct wval_fun {
  char type;     // wasm offset: 0
  char subtype;  // 1

  short fn_table_index;  // 2
  short param_count;     // 4
  short has_rest_arg;    // 6
  short partial_count;   // 8
  int closure;           // 12
  int partials;          // 16
  /* char* str;             // 20 */
};

struct lenv {
  int is_ns_env;
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

typedef struct {
  char* namespace;

  Map as;  // alias -> namespace
  //{"foo": "foo.core", "foo.core": "foo.core"}
  Map refer;  // symbol -> namespace
  //{"bar": "foo.core", "bax": "foo.core"}
} Namespace;

typedef struct {
  /* {"namespace.name": (Lenv*){*ns*: (Namespace){(Map){foo.core: env, bar.core:
   * env}}}} */
  // Cache:
  Map namespaces;
} State;

/* lval types */
enum {
  LVAL_SYMBOL,
  LVAL_COLLECTION,
  LVAL_LITERAL,
  LVAL_FUNCTION,
  LVAL_ERR,
  LVAL_NAMESPACE,
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
