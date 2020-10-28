#ifndef __LTYPES_H_
#define __LTYPES_H_

#define MAX_FN_PARAMS 20

typedef struct {
  char* src;   // root dir of src files
  char* main;  // namespace containing main fn
  char* stdlib;
  char* out_wasm;
  char* builtin;
  char* user;  // user ns, dummy namespace
  char* out;   // dir of compiled wasm files
  int do_compile;
  // when root fns get compiled there are 2 versions:
  // 1. A fn where the params match the wajure params, which is called directly
  // when possible
  // 2. A fn that uses the standard abi of closure_ptr, args_block_ptr and
  // args_count, used in datafied versions of the fn (partial f 1) or (let [g
  // f]).
  // if reuse_wajure_fn is truthy the first fn is called from the second. Else
  // the first fn is inlined, which might be slightly faster.
  int reuse_wajure_fn;
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

typedef Lval* (*Lbuiltin)(Lenv*, Lval*);

// Used to store data on runtime builtin fns
typedef struct {
  char* wajure_fn_name;
  Lbuiltin fun;
  char* c_fn_name;
  int params_count;
  int results_count;
  int fn_table_index;
  int data_offset;
} CFn;

// TODO: refactor!!!
typedef struct context Context;

typedef struct namespace Namespace;

typedef union {
  long num;
  char* str;
  Cell* head;
} Data;

typedef struct {
  Lval* params;
  int param_count;
  int has_rest_arg;
  Lval* body;
  Lval* err;
  char* name;  // malloced!!!
} Lambda;

#ifdef WASM

struct lval {
  char type;     // more like the protocol (collection, function, literal etc)
  char subtype;  // more like the actual type (list, map, vector, lambda, macro
                 // etc)
  Data data;

  int hash;

  // fn
  short fn_table_index;  // TODO: max of about 65000 (2^16) fns, possible make
                         // int?
  short partial_count;
  int closure;
  int partials;
};
#else
struct lval {
  char type;
  char subtype;

  Data data;
  int hash;

  // Interpreter props
  Lbuiltin c_fn;  // builtin c fns == sys fns

  Lambda** lambdas;  // multi arity fns,
  Lenv* closure;
  Cell* partials;

  // We need to keep track of in which namespace a fn is defined and under which
  // name so we can refer to its fn_table_index from other namespaces in
  // compiled code by imported global.
  Namespace* ns;
  char* cname;  // final_binding, or wasm_name, or canonical name, or actual
                // name rather than an alias

  // Ptr to canonical fn for partial fns, the fn that has the cname prop that we
  // actually need to call in wasm when calling the partial fn derived from it.
  Lval* cfn;

  // Compiler to wasm data
  int data_offset;  // offset of datafied lval relative to module's data offset
  Context* context;
  int fn_table_index;  // fn_table_index, local_index or closure offset.
  int local_index;
};
#endif

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
  // Keep track of symbols (params and let bindings) as they are encountered
  // when compiling a fn that are NOT local, but from a parent fn.
  /* int symbol_count; */
  Cell* symbol_to_ref;
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

  Cell* wajure_to_c_fn_map;
  Cell* wajure_to_native_fn_map;
} State;

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
  LOCAL

};

// FTI = Function Table Index
// These FTI_ constants need to match the builtin array in builtin.c
enum {
  FTI_RTE_NOT_A_FN,
  FTI_COPY_AND_RETAIN,
  FTI_PARTIAL,
  FTI_APPLY,
  FTI_KEYWORD,
  FTI_VECTOR,
  FTI_SET,
  FTI_SYMBOL,
  FTI_MAP,
};

// Compiler
enum { TOO_MANY_ARGS, TOO_FEW_ARGS, ARGS_MATCH_PARAMS };

#endif  // __LTYPES_H_
