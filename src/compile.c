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
#include "refcount.h"
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

BinaryenExpressionRef compile_symbol(Wasm* wasm, Lval* lval_symbol) {
  Lval* lval_resolved_sym = lenv_get(wasm->env, lval_symbol);

  printf("resolved sym: \n");
  lval_println(lval_resolved_sym);

  return lval_compile(wasm, lval_resolved_sym);
}

BinaryenExpressionRef make_lval_literal(Wasm* wasm, Lval* lval);

BinaryenExpressionRef compile_fn_call(Wasm* wasm, Lval* lval_list) {
  BinaryenModuleRef module = wasm->module;

  if (lval_list->head == NIL) {
    if (!wasm->lval_empty_list_offset)
      wasm->lval_empty_list_offset = make_lval_literal(wasm, lval_list);
    return wasm->lval_empty_list_offset;
  }

  Lval* lval_first = lval_list->head->car;
  switch (lval_first->type) {
    case LVAL_SYMBOL:;
      Lval* lval_fn = lenv_get(wasm->env, lval_first);

      ;  // TODO: resolve symbol
    case LVAL_COLLECTION:
      switch (lval_first->subtype) {
        case LIST:
          return compile_fn_call(wasm, lval_first);  // TODO: ?????
        case VECTOR:
          // TODO: Error!!!!
        case MAP:
          // TODO: Error!!!!
        default:;
          /* return make_lval_err("Unknown seq subtype: %d ", lval->subtype); */
      }
      break;
  }
  /* scoped Lval* lval_fun = lval_compile(env, lval_first); */

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

int slot_type_size = 4 * 4;
int lval_type_size = 5 * 4;  // type and subtype are in 4 bytes
int ref_count_offset = 0;
int data_p_offset = 3;
int type_offset = 4;
int num_offset = 5;
int str_offset = 6;
int head_offset = 7;
int hash_offset = 8;

BinaryenExpressionRef make_lval_literal(Wasm* wasm, Lval* lval) {
  int data_lval_size = slot_type_size + lval_type_size;
  int* data_lval = calloc(1, data_lval_size);
  int string_offset = 0;
  if (lval->str) string_offset = add_string_to_data(wasm, lval->str);

  data_lval[ref_count_offset] = 1;
  data_lval[data_p_offset] = wasm->__data_end + wasm->strings_offset + 16;
  data_lval[type_offset] = lval->type | lval->subtype << 8;
  data_lval[num_offset] = lval->num;
  data_lval[str_offset] = wasm->__data_end + string_offset;
  data_lval[head_offset] = 0;  //(int)lval->head;  // TODO: add list to data!!!
  data_lval[hash_offset] = lval->hash;

  int offset = add_bytes_to_data(wasm, (char*)data_lval, data_lval_size);

  return make_int32(wasm->module, wasm->__data_end + offset + slot_type_size);
}

BinaryenExpressionRef compile_lval_literal(Wasm* wasm, Lval* lval) {
  switch (lval->subtype) {
    /* case STRING:; */
    /* case KEYWORD:; */
    /* case SYMBOL:; */
    case NUMBER:;
      if (lval->num >= wasm->lval_num_start &&
          lval->num <= wasm->lval_num_end) {
        if (!wasm->lval_num_offset[lval->num - wasm->lval_num_start])
          wasm->lval_num_offset[lval->num - wasm->lval_num_start] =
              make_lval_literal(wasm, lval);

        return wasm->lval_num_offset[lval->num - wasm->lval_num_start];
      }

      return make_lval_literal(wasm, lval);
    case LTRUE:
      if (!wasm->lval_true_offset)
        wasm->lval_true_offset = make_lval_literal(wasm, lval);
      return wasm->lval_true_offset;
    case LFALSE:
      if (!wasm->lval_false_offset)
        wasm->lval_false_offset = make_lval_literal(wasm, lval);
      return wasm->lval_false_offset;
    case LNIL:
      if (!wasm->lval_nil_offset)
        wasm->lval_nil_offset = make_lval_literal(wasm, lval);
      return wasm->lval_nil_offset;
    default:
      return make_lval_literal(wasm, lval);
  }
}

BinaryenExpressionRef lval_compile(Wasm* wasm, Lval* lval) {
  /* printf("lval_compile: "); */
  /* lval_println(lval); */
  /* printf("lval->type: %s\n", lval_type_constant_to_name(lval->type)); */
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

void compile_function(Wasm* wasm, Lval* lval_sym, Lval* lval) {
  /* BinaryenModuleRef module = wasm->module; */

  printf("fn-name: %s\n", lval_sym->str);
  printf("params: ");
  lval_println(lval->params);
  printf("body: ");
  lval_println(lval->body);
  char* fn_name = lval_sym->str;

  int params_count = 0;
  int locals_count = 0;
  int results_count = 1;

  BinaryenExpressionRef arg_list_head =
      compile_do_list(wasm, lval->body, RETURN_ON_ERROR);
  /* compile_fn_rest_args(wasm, lval->body->head); */
  /* BinaryenExpressionRef drop = call_indirect_example(wasm); */

  /* BinaryenExpressionRef lval_list_operands[1] = {arg_list_head}; */
  /* BinaryenExpressionRef lval_list = BinaryenCall( */
  /*     module, "new_lval_list", lval_list_operands, 1, make_type_int32(1)); */

  /* BinaryenExpressionRef lval_println_operands[1] = {lval_list}; */
  /* BinaryenExpressionRef lval_println_operands[1] = {wasm_offset(wasm, 22)};
   */
  /* BinaryenExpressionRef body = BinaryenCall( */
  /*     module, "lval_println", lval_println_operands, 1, BinaryenTypeNone());
   */

  /* BinaryenExpressionRef print_slot_size = */
  /*     BinaryenCall(module, "print_slot_size", NULL, 0, BinaryenTypeNone());
   */
  /* body = print_slot_size; */

  BinaryenExpressionRef body = arg_list_head;
  lval->offset = add_fn_to_table(wasm, fn_name);
  add_wasm_function(wasm, fn_name, params_count, locals_count, body,
                    results_count);
}

void print_pair(Lval* lval_sym, Lval* lval) {
  lval_print(lval_sym);
  printf(": ");
  lval_print(lval);
}

void process_env(Wasm* wasm) {
  Cell* cell = wasm->env->kv;
  if (!cell || !cell->car) return;
  printf("Processing env ==============================\n");
  while (cell) {
    Cell* pair = cell->car;
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;

    /* print_pair(lval_sym, lval); */
    if (lval->type == LVAL_FUNCTION) {
      compile_function(wasm, lval_sym, lval);
    }

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

/* int __data_end; */

int compile(char* file_name) {
  info("Compiling %s\n", file_name);
  /* print_slot_size(); */

  set_log_level(LOG_LEVEL_INFO);

  Wasm* wasm = init_wasm();
  /* printf("__data_end: %d\n", wasm->__data_end); */
  wasm->env = interprete_file(file_name);

  import_runtime(wasm);

  process_env(wasm);

  BinaryenAddFunctionExport(wasm->module, "test", "test");

  add_fn_to_table(wasm, "log_int");
  add_fn_to_table(wasm, "printf_");
  // We only have this data _after_ compiling
  /* add_global_section(module); */
  add_function_table(wasm);

  /* add_string_to_data(wasm, "foo3"); */
  add_memory_section(wasm);

  int validate_result = BinaryenModuleValidate(wasm->module);
  if (!validate_result)
    printf("VALIDATE ERROR!!!!!!\n");
  else
    printf("VALIDATED OK!!\n");
  /* BinaryenModulePrint(wasm->module); */

  write_wat(wasm, "compiled/lispy.wat");
  write_wasm(wasm, "compiled/lispy.wasm");

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

/* add_string_to_data(wasm, "foo3"); */
/* add_string_to_data(wasm, "bar"); */

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

/* BinaryenExpressionRef compile_lval_literal(Wasm* wasm, Lval* lval) { */
/*   BinaryenModuleRef module = wasm->module; */
/*   /\* printf("lval_compile literal: "); *\/ */
/*   BinaryenExpressionRef* operands = NULL; */
/*   char* func_name; */
/*   int operands_count = 1; */
/*   switch (lval->subtype) { */
/*     case NUMBER:; */
/*     case STRING:; */
/*       return compile_lval_literal(wasm, lval); */
/*       /\* int len = _strlen(lval->str); *\/ */
/*       /\* BinaryenExpressionRef wasm_len = make_int32(module, len); *\/ */
/*       /\* BinaryenExpressionRef operands_lalloc[1] = {wasm_len}; *\/ */
/*       /\* BinaryenExpressionRef lalloc_size = BinaryenCall( *\/ */
/*       /\*     module, "lalloc_size", operands_lalloc, 1, make_type_int32(1));
 * *\/ */

/*       /\* int offset = add_string_to_data(wasm, lval->str); *\/ */

/*       /\* BinaryenExpressionRef operands_strcpy[2] = {lalloc_size, *\/ */
/*       /\*                                             wasm_offset(wasm,
 * offset)}; */
/*        *\/ */
/*       /\* BinaryenExpressionRef _strcpy = BinaryenCall( *\/ */
/*       /\*     module, "_strcpy", operands_strcpy, 2, make_type_int32(1)); *\/
 */
/*       /\* BinaryenExpressionRef operands_string[1] = {_strcpy}; *\/ */

/*       /\* operands = operands_string; *\/ */
/*       /\* func_name = "make_lval_str"; *\/ */
/*       /\* break; *\/ */
/*     case LNIL: */
/*       operands_count = 0; */
/*       func_name = "make_lval_nil"; */
/*       break; */
/*     case LTRUE: */
/*       operands_count = 0; */
/*       func_name = "make_lval_true"; */
/*       break; */
/*     case LFALSE: */
/*       operands_count = 0; */
/*       func_name = "make_lval_false"; */
/*       break; */
/*     default: */
/*       return make_int32(module, 123); */
/*   } */

/*   BinaryenExpressionRef literal = BinaryenCall( */
/*       module, func_name, operands, operands_count, make_type_int32(1)); */

/*   return literal; */
/* } */

/* printf("offset: %d\n", wasm->strings_offset); */
/* /\* printf("S2 %s:\n", s2); *\/ */
/* char* s = (char*)data_lval; */

/* for (int i = 0; i < 14; i++) printf("%x,", data_lval[i]); */
/* printf("\n"); */
/* for (int i = 0; i < 36; i++) { */
/*   if (i % 4 == 0) printf("|"); */
/*   printf("%x,", s[i]); */
/* } */
