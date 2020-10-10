#ifndef __LTYPES_H_
#define __LTYPES_H_

#define MAX_FN_PARAMS 20

typedef struct {
  char* src;     // root dir of src files
  char* main;    // namespace containing main fn
  char* stdlib;  // stdlib namespace
  char* user;    // user ns, dummy namespace
  char* out;     // dir of compiled wasm files
  int do_compile;
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
/* typedef struct wval_fn WvalFun; */

typedef Lval* (*Lbuiltin)(Lenv*, Lval*);

// Used to store data on runtime builtin fns
typedef struct {
  char* wajure_fn_name;
  Lbuiltin fun;
  char* c_fn_name;
  int params_count;
  int results_count;
} RuntimeFn;

// TODO: refactor!!!
typedef struct context Context;

typedef struct namespace Namespace;

typedef union {
  long num;
  char* str; /* Function */

  // List
  Cell* head;
} Data;

#ifdef WASM

struct lval {
  char type;     // more like the protocol (collection, function, literal etc)
  char subtype;  // more like the actual type (list, map, vector, lambda, macro
                 // etc)

  // Number, error, symbol or string
  /* char* str; /\* Function *\/ */

  // List
  Cell* head;

  Data data;

  int hash;
  // fn
  short fn_table_index;
  short partial_count;
  int closure;
  int partials;
  int fn_call_relay;
  /* int rest_arg_index ??*/
};
#else
struct lval {
  char type;
  char subtype;

  // Number, error, symbol or string
  /* long num; */
  /* char* str; /\* Function *\/ */

  // List
  Cell* head;

  Data data;
  int hash;

  // Interpreter props
  Lbuiltin fun;
  Lenv* closure;
  Lval* params;
  Lval* body;
  Cell* partials;
  int param_count;
  int rest_arg_index;
  // We need to keep track of in which namespace a fn is defined and under which
  // name so we can refer to its fn_table_index from other namespaces in
  // compiled code by imported global.
  Namespace* ns;
  char* cname;  // final_binding, or wasm_name, or canonical name, or actual
                // name rather than an alias

  // Compiler to wasm data
  int wval_ptr;
  Context* context;
  int offset;

  // Ptr to canonical fn for partial fns, the fn that has the cname prop that we
  // actually need to call in wasm when calling the partial fn derived from it.
  Lval* cfn;
};
#endif

// Used in wasm runtime. We stuff info on a wasm lambda into a lval
/* struct wval_fn { */
/*   char type;     // wasm offset: 0 */
/*   char subtype;  // 1 */

/*   short fn_table_index;  // 2 */
/*   short param_count;     // 4 */
/*   short has_rest_arg;    // 6 */
/*   short partial_count;   // 8 */
/*   int closure;           // 12 */
/*   int partials;          // 16 */
/*   int fn_call_relay_array; */
/*   /\* char* str;             // 20 *\/ */
/* }; */

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

struct namespace {
  char* namespace;
  Lenv* env;
  Map as;  // alias -> namespace
  //{"foo": "foo.core", "foo.core": "foo.core"}
  Map refer;  // symbol -> namespace
  //{"bar": "foo.core", "bax": "foo.core"}
  Map required;
  int compile;
  // dependants in the sense that the interpreter (when loading this namespace)
  // is resolving symbols from another namespace. So these do not include
  // references to external symbols in functions that are not run during
  // loading. This is a sign that these dependant namespaces will have to be
  // recompiled if this namespace is recompiled, since the values that the
  // interpreter calculates, other than fns, are hardcoded in the dependant
  // module's data section..
  Map dependants;
  int uid_counter;
};

typedef struct {
  Lenv* builtins_env;
  Namespace* current_ns;
  Namespace* stdlib_ns;
  // Cache:
  Map namespaces;
  /* {"namespace.name": Namespace* ns} */
  int mark_deps;
} State;

struct resolved_symbol {
  Lval* lval;
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
  LVAL_REF,
  LVAL_EXTERNAL,  // a required symbol
  // local ref subtypes
  PARAM,
  LOCAL,

  // wajure runtime lval fn type
  WVAL_FUN
};

// Compiler
enum { TOO_MANY_ARGS, TOO_FEW_ARGS, ARGS_MATCH_PARAMS };

#endif  // __LTYPES_H_
