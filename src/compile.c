#include "compile.h"

#include <binaryen-c.h>
#include <stdlib.h>

#include "env.h"
#include "fns.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "list.h"
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

BinaryenExpressionRef lval_compile(Wasm* wasm, Lval* lval);

enum { RETURN_ON_ERROR, RETURN_LAST_EVAL };

BinaryenExpressionRef compile_fn_rest_args(Wasm* wasm, Cell* head) {
  BinaryenModuleRef module = wasm->module;
  if (!head) {
    return make_int32(module, 0);
  }
  BinaryenExpressionRef literal = lval_compile(wasm, head->car);
  BinaryenExpressionRef operands[] = {literal,
                                      compile_fn_rest_args(wasm, head->cdr)};
  return BinaryenCall(module, "list_cons", operands, 2, make_type_int32(1));
}

BinaryenExpressionRef compile_do_list(Wasm* wasm, Lval* list, int mode) {
  lval_println(list);
  BinaryenModuleRef module = wasm->module;
  scoped_iter Cell* i = iter_new(list);
  Lval* lval = iter_next(i);
  int count = list_count(list->head);
  BinaryenExpressionRef* do_list =
      malloc(count * sizeof(BinaryenExpressionRef));

  /* int result = 0; */
  int do_list_count = 0;

  while (lval) {
    BinaryenExpressionRef literal = lval_compile(wasm, lval);
    if (do_list_count + 1 < count) literal = BinaryenDrop(module, literal);

    do_list[do_list_count++] = literal;

    /* if (result->type == LVAL_ERR) { */
    /*   lval_print(lval); */
    /*   printf(" resulted in: "); */
    /*   lval_println(result); */
    /* } */
    /* return result; */
    lval = iter_next(i);
  }

  return BinaryenBlock(module, "body", do_list, do_list_count,
                       BinaryenTypeAuto());
}

BinaryenExpressionRef compile_symbol(Wasm* wasm, Lval* lval) {
  /* Lval* lval_resolved_sym = lenv_get(env, lval_symbol); */
  /* return lval_resolved_sym; */

  BinaryenModuleRef module = wasm->module;
  return make_int32(module, 0);
}

/* BinaryenExpressionRef compile_symbol_literal(Wasm* wasm, Lval* lval) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   int len = _strlen(lval->str); */
/*   BinaryenExpressionRef wasm_len = make_int32(module, len); */
/*   BinaryenExpressionRef operands_lalloc[1] = {wasm_len}; */
/*   BinaryenExpressionRef lalloc_size = BinaryenCall( */
/*       module, "lalloc_size", operands_lalloc, 1, make_type_int32(1)); */

/*   // TODO: see if str already exists and return that offset instead of adding
 */
/*   // str again to data */
/*   int offset = add_string_to_data(wasm, lval->str); */

/*   BinaryenExpressionRef operands_strcpy[2] = {lalloc_size, */
/*                                               wasm_offset(wasm, offset)}; */
/*   BinaryenExpressionRef _strcpy = */
/*       BinaryenCall(module, "_strcpy", operands_strcpy, 2,
 * make_type_int32(1)); */
/*   BinaryenExpressionRef operands[1] = {_strcpy}; */

/*   return BinaryenCall(module, "make_lval_sym", operands, 1,
 * make_type_int32(1)); */
/* } */

BinaryenExpressionRef compile_fn_call(Wasm* wasm, Lval* lval) {
  BinaryenModuleRef module = wasm->module;
  return make_int32(module, 0);
}

BinaryenExpressionRef compile_vector(Wasm* wasm, Lval* lval) {
  BinaryenModuleRef module = wasm->module;
  return make_int32(module, 0);
}

BinaryenExpressionRef compile_map(Wasm* wasm, Lval* lval) {
  BinaryenModuleRef module = wasm->module;
  return make_int32(module, 0);
}

BinaryenExpressionRef compile_lval_literal(Wasm* wasm, Lval* lval) {
  BinaryenModuleRef module = wasm->module;
  /* printf("lval_compile literal: "); */
  BinaryenExpressionRef* operands = NULL;
  char* func_name;
  int operands_count = 1;
  switch (lval->subtype) {
    case NUMBER:;
      BinaryenExpressionRef operands_num[1] = {make_int32(module, lval->num)};
      func_name = "make_lval_num";
      operands = operands_num;
      break;
    case STRING:;
      int len = _strlen(lval->str);
      BinaryenExpressionRef wasm_len = make_int32(module, len);
      BinaryenExpressionRef operands_lalloc[1] = {wasm_len};
      BinaryenExpressionRef lalloc_size = BinaryenCall(
          module, "lalloc_size", operands_lalloc, 1, make_type_int32(1));

      int offset = add_string_to_data(wasm, lval->str);

      BinaryenExpressionRef operands_strcpy[2] = {lalloc_size,
                                                  wasm_offset(wasm, offset)};
      BinaryenExpressionRef _strcpy = BinaryenCall(
          module, "_strcpy", operands_strcpy, 2, make_type_int32(1));
      BinaryenExpressionRef operands_string[1] = {_strcpy};

      operands = operands_string;
      func_name = "make_lval_str";
      break;
    case LNIL:
      operands_count = 0;
      func_name = "make_lval_nil";
      break;
    case LTRUE:
      operands_count = 0;
      func_name = "make_lval_true";
      break;
    case LFALSE:
      operands_count = 0;
      func_name = "make_lval_false";
      break;
    default:
      return make_int32(module, 123);
  }

  BinaryenExpressionRef literal = BinaryenCall(
      module, func_name, operands, operands_count, make_type_int32(1));

  return literal;
}

BinaryenExpressionRef lval_compile(Wasm* wasm, Lval* lval) {
  printf("lval_compile: ");
  lval_println(lval);
  printf("lval->type: %s\n", lval_type_constant_to_name(lval->type));
  /* BinaryenExpressionRef ref = NULL; */
  switch (lval->type) {
    case LVAL_SYMBOL:
      return compile_symbol(wasm, lval);
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:
          return compile_fn_call(wasm, lval);
          break;
        case VECTOR:
          return compile_vector(wasm, lval);
          break;
        case MAP:
          /* TODO: */
          return compile_map(wasm, lval);
          break;
        default:;
          /* return make_err("Unknown seq subtype: %d ", lval->subtype); */
      }
      break;
    default:
      return compile_lval_literal(wasm, lval);
  }
  return NULL;
}

void add_wasm_function(Wasm* wasm, char* fn_name, int params_count,
                       int locals_count, BinaryenExpressionRef body,
                       int results_count) {
  BinaryenModuleRef module = wasm->module;

  BinaryenType params_type = make_type_int32(params_count);
  BinaryenType results_type = make_type_int32(results_count);
  /* BinaryenType results_type = make_results_type(results_count); */

  BinaryenType* local_types = make_type_int32_array(locals_count);

  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, body);
}

void process_function(Wasm* wasm, Lval* lval_sym, Lval* lval) {
  BinaryenModuleRef module = wasm->module;

  printf("fn-name: %s\n", lval_sym->str);
  printf("params: ");
  lval_println(lval->params);
  printf("body: ");
  lval_println(lval->body);
  char* fn_name = lval_sym->str;

  int params_count = 0;
  int locals_count = 0;
  int results_count = 0;

  BinaryenExpressionRef arg_list_head =
      /* compile_do_list(wasm, lval->body, RETURN_ON_ERROR); */
      compile_fn_rest_args(wasm, lval->body->head);
  /* BinaryenExpressionRef drop = call_indirect_example(wasm); */

  BinaryenExpressionRef lval_list_operands[1] = {arg_list_head};
  BinaryenExpressionRef lval_list = BinaryenCall(
      module, "new_lval_list", lval_list_operands, 1, make_type_int32(1));

  BinaryenExpressionRef lval_println_operands[1] = {lval_list};
  BinaryenExpressionRef body = BinaryenCall(
      module, "lval_println", lval_println_operands, 1, BinaryenTypeNone());

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
  printf("Processing env ==============================\n");
  while (cell) {
    Cell* pair = cell->car;
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;

    /* print_pair(lval_sym, lval); */
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

  process_env(wasm, env);

  BinaryenAddFunctionExport(wasm->module, "test", "test");

  add_fn_to_table(wasm, "log_int");
  add_fn_to_table(wasm, "printf_");
  // We only have this data _after_ compiling
  /* add_global_section(module); */
  add_function_table(wasm);

  /* add_string_to_data(wasm, "foo3"); */
  /* add_string_to_data(wasm, "bar"); */
  add_memory_section(wasm);

  printf("validate result: %d\n", BinaryenModuleValidate(wasm->module));
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

/* BinaryenExpressionRef call_indirect_example(Wasm* wasm) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   BinaryenExpressionRef fn_index = make_int32(module, 0); */
/*   BinaryenExpressionRef operands[] = {wasm_offset(wasm, 5), */
/*                                       make_int32(module, 0)}; */

/*   BinaryenType params_type = make_type_int32(2); */

/*   BinaryenType results_type = make_type_int32(0); */

/*   BinaryenExpressionRef call_indirect = BinaryenCallIndirect( */
/*       module, fn_index, operands, 2, params_type, results_type); */

/*   BinaryenExpressionRef drop = BinaryenDrop(module, call_indirect); */
/*   return drop; */
/* } */

/* struct table_fn { */
/*   char* fn_name; */
/*   int table_index; */
/*   int operands_count; */
/*   BinaryenType operands_type; */
/*   int results_count; */
/* }; */
