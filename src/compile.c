#include "compile.h"

#include <binaryen-c.h>

#include "env.h"
#include "fns.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "misc_fns.h"
#include "print.h"
/* #include "misc_fns.h" */
/* #include "print.h" */
#include "read.h"
/* #include "refcount.h" */
#include "wasm.h"
#include "wasm_util.h"

// Builtins:
// special: do, let, if, try, throw, quote, quasiquote
// maths: +, -, *, /, >, < ,<=, >=, =, not=
// list: cons, first, list, rest, concat, count, nth, list?
// misc: exit, print, boolean, hash

char* make_err(char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  char* str = lalloc_size(512);
  vsnprintf(str, 511, fmt, va);
  str = lrealloc(str, _strlen(str) + 1);
  va_end(va);
  return str;
}

int compile_expression(BinaryenModuleRef module, Lval* lval_expr) {
  printf("LVAL:");
  lval_println(lval_expr);
  /* info("\n->>>>>>>>EVAL: "); */
  /* lval_infoln(lval); */
  /* printf("lval->type: %s\n", lval_type_constant_to_name(lval->type)); */
  /* Lval* ret = NIL; */
  /* switch (lval->type) { */
  /*   case LVAL_SYMBOL: */
  /*     return eval_symbol(env, lval); */
  /*   case LVAL_COLLECTION: */
  /*     switch (lval->subtype) { */
  /*       case LIST: */
  /*         return compile_fn_call(env, lval); */
  /*       case VECTOR: */
  /*         return compile_vector(env, lval); */
  /*       case MAP: */
  /*         /\* TODO: *\/ */
  /*         return lval; */
  /*       default: */
  /*         return make_err("Unknown seq subtype: %d ", lval->subtype); */
  /*     } */
  /*     break; */
  /*   default: */
  /*     return retain(lval); */
  /* } */
  return 1;
}

BinaryenType* make_type_int32_array(int count) {
  BinaryenType* types = malloc(count * sizeof(BinaryenTypeInt32()));
  while (count--) types[count] = BinaryenTypeInt32();
  return types;
}

void add_wasm_function(Wasm* wasm, char* fn_name, int params_count,
                       int locals_count, BinaryenExpressionRef body,
                       int results_count) {
  BinaryenModuleRef module = wasm->module;

  /* BinaryenType param_types = TypeNone; */
  BinaryenType* _params_type = make_type_int32_array(params_count);
  BinaryenType param_types = BinaryenTypeCreate(_params_type, params_count);

  BinaryenType* _results_type = make_type_int32_array(results_count);
  BinaryenType results_type = BinaryenTypeCreate(_results_type, results_count);
  /* BinaryenType result_type = TypeNone; */
  /* BinaryenType result_type = TypeInt32x1; */

  BinaryenType* local_types = make_type_int32_array(locals_count);

  BinaryenAddFunction(module, fn_name, param_types, results_type, local_types,
                      locals_count, body);
}

void process_function(Wasm* wasm, Lval* lval_sym, Lval* lval) {
  BinaryenModuleRef module = wasm->module;

  char* fn_name = lval_sym->str;

  /* BinaryenType param_types = TypeNone; */
  int params_count = 1;
  int locals_count = 3;
  int results_count = 0;

  BinaryenExpressionRef log_int = wasm_log_int(wasm, 123);
  BinaryenExpressionRef body_list[] = {log_int};

  int body_list_count = sizeof(body_list) / sizeof(BinaryenExpressionRef);
  BinaryenExpressionRef body = BinaryenBlock(
      module, "body", body_list, body_list_count, BinaryenTypeAuto());

  lval->offset = add_fn_to_table(wasm, fn_name);
  add_wasm_function(wasm, fn_name, params_count, locals_count, body,
                    results_count);
}

void print_pair(Lval* lval_sym, Lval* lval) {
  lval_print(lval_sym);
  printf(": ");
  lval_print(lval);
}

void process_env(Wasm* wasm, Lenv* env) {
  Cell* cell = env->kv;
  if (!cell || !cell->car) return;
  while (cell) {
    Cell* pair = cell->car;
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;

    print_pair(lval_sym, lval);
    if (lval->type == LVAL_FUNCTION) {
      process_function(wasm, lval_sym, lval);
    }
    /* else { */
    /*   int offset = write_lval_to_data(wasm, lval); */
    /*   lval->offset = offset; */
    /* } */
    cell = cell->cdr;
    if (cell) printf("%s", "\n");
  }
  printf("\n");

  // walk list and encode every non function into a wasm data string, and keep
  // tabs on the offset for each symbol.

  // For functions: create a wasm function for each. Any symbol encountered
  // resolve to lval in wasm data by offset, unless redefined in fn already.
  // Any functions encountered again create a wasm function, but now with
  // closure vars added.
}

/* BinaryenExpressionRef BinaryenCall(BinaryenModuleRef module, */
/*                                    const char* target, */
/*                                    BinaryenExpressionRef* operands, */
/*                                    BinaryenIndex numOperands, */
/*                                    BinaryenType returnType); */

/* BINARYEN_API BinaryenExpressionRef */
/* BinaryenCallIndirect(BinaryenModuleRef module, */
/*                      BinaryenExpressionRef target, */
/*                      BinaryenExpressionRef* operands, */
/*                      BinaryenIndex numOperands, */
/*                      BinaryenType params, */
/*                      BinaryenType results); */

int compile(char* file_name) {
  info("Compiling %s\n", file_name);

  set_log_level(LOG_LEVEL_INFO);
  Lenv* env = interprete_file(file_name);
  Wasm* wasm = init_wasm();
  import_runtime(wasm);

  printf("==================================================\n");

  process_env(wasm, env);

  printf("==================================================\n");

  BinaryenAddFunctionExport(wasm->module, "test", "test");

  // We only have this data _after_ compiling
  /* add_global_section(module); */
  add_function_table(wasm);
  add_memory_section(wasm);

  /* BinaryenModulePrint(wasm->module); */

  write_wat(wasm, "compiled/lispy.wat");
  write_wasm(wasm, "compiled/lispy.wasm");

  release_env(env);
  free_wasm(wasm);
  return 1;
}

/* Lval* parse_file(char* file_name) { */
/*   char* str = read_file(file_name); */
/*   if (!str) { */
/*     printf("Could not load file %s", str); */
/*     return NULL; */
/*   } */

/*   int pos = 0; */
/*   Lval* lval_list = lval_read_list(str, &pos, '\0'); */
/*   if (lval_list->type == LVAL_ERR) { */
/*     lval_println(lval_list); */
/*     return NULL; */
/*   } */
/*   return lval_list; */
/* } */

/* void add_global_section(BinaryenModuleRef module) { */
/*   BinaryenAddGlobalImport(module, "data_end", "env", "data_end", */
/*                           BinaryenTypeInt32(), 0); */

/*   BinaryenAddGlobal(module, "a-global", BinaryenTypeInt32(), 0, */
/*                     make_int32(module, 7)); */

/*   BinaryenAddGlobal(module, "a-mutable-global", BinaryenTypeFloat32(), 1,
 */
/*                     make_int32(module, 123)); */
/* } */
