#include "compile.h"

#include <binaryen-c.h>

#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "misc_fns.h"
#include "print.h"
#include "read.h"
#include "refcount.h"

BinaryenExpressionRef make_int32(BinaryenModuleRef module, int x) {
  return BinaryenConst(module, BinaryenLiteralInt32(x));
}

/* BinaryenExpressionRef make_memory_init(BinaryenModuleRef module) { */
/*   BinaryenExpressionRef dest = make_int32(module, 1836); */
/*   BinaryenExpressionRef offset = make_int32(module, 0); */
/*   BinaryenExpressionRef size = make_int32(module, 12); */
/*   return BinaryenMemoryInit(module, 1, dest, offset, size); */
/* }; */

char* make_err(char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  char* str = lalloc_size(512);
  vsnprintf(str, 511, fmt, va);
  str = lrealloc(str, _strlen(str) + 1);
  va_end(va);
  return str;
}

// import: memory, table, data_end, make_lval

int compile_expression(BinaryenModuleRef module, Lval* lval_expr) {
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

typedef struct {
  BinaryenModuleRef module;
  char* strings;
  int strings_offset;
} Wasm;

Wasm* init_wasm() {
  Wasm* wasm = malloc(sizeof(Wasm));
  wasm->module = BinaryenModuleCreate();
  wasm->strings = malloc(1);
  wasm->strings_offset = 0;
  return wasm;
}
void free_wasm(Wasm* wasm) {
  BinaryenModuleDispose(wasm->module);
  free(wasm->strings);
  free(wasm);
}

int strings_data_cat(Wasm* wasm, char* str) {
  int len = _strlen(str) + 1;
  /* printf("%d %d\n", wasm->strings_offset, len); */
  wasm->strings = realloc(wasm->strings, wasm->strings_offset + len);
  _strncpy(wasm->strings + wasm->strings_offset, str, len);
  wasm->strings_offset += len;
  /* printf("strings_offset: %d\n", wasm->strings_offset); */
  return wasm->strings_offset;
}

void add_memory_section(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  BinaryenAddGlobalImport(module, "__data_end", "env", "__data_end",
                          BinaryenTypeInt32(), 0);
  BinaryenAddMemoryImport(module, "memory", "env", "memory", 0);
  const int num_segments = 1;
  const char* segments[1] = {wasm->strings};
  BinaryenIndex segmentSizes[] = {wasm->strings_offset};

  BinaryenExpressionRef __data_end =
      BinaryenGlobalGet(module, "__data_end", BinaryenTypeInt32());
  BinaryenExpressionRef segmentOffsets[1] = {__data_end};
  int8_t segmentPassive[] = {0};

  int initial_mem_size = 2;
  /* int max_mem_size = 32767; */
  int max_mem_size = 65536;
  int shared = 0;
  BinaryenSetMemory(module, initial_mem_size, max_mem_size, "mem", segments,
                    segmentPassive, segmentOffsets, segmentSizes, num_segments,
                    shared);
}

/* void add_global_section(BinaryenModuleRef module) { */
/*   BinaryenAddGlobalImport(module, "data_end", "env", "data_end", */
/*                           BinaryenTypeInt32(), 0); */

/*   BinaryenAddGlobal(module, "a-global", BinaryenTypeInt32(), 0, */
/*                     make_int32(module, 7)); */

/*   BinaryenAddGlobal(module, "a-mutable-global", BinaryenTypeFloat32(), 1, */
/*                     make_int32(module, 123)); */
/* } */

void add_function_table(BinaryenModuleRef module) {
  BinaryenAddTableImport(module, "table", "env", "table");
  int initial = 1;
  int maximum = 1;

  const char* funcNames[] = {"some_func"};
  int numFuncNames = 1;
  BinaryenExpressionRef offset = make_int32(module, 11);
  BinaryenSetFunctionTable(module, initial, maximum, funcNames, numFuncNames,
                           offset);
}

int compile_list(Wasm* wasm, Lval* list) {
  BinaryenModuleRef module = wasm->module;
  scoped_iter Cell* i = iter_new(list);
  Lval* lval = iter_next(i);
  int result;
  while (lval) {
    result = compile_expression(module, lval);
    if (!result) {
      lval_print(lval);
      return result;
    }
    lval = iter_next(i);
  }
  return 1;
}

BinaryenType TypeInt32;
BinaryenType TypeInt32x1;
BinaryenType TypeInt32x2;
BinaryenType TypeNone;

void import_runtime(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  TypeInt32 = BinaryenTypeInt32();
  BinaryenType _TypeInt32x1[1] = {BinaryenTypeInt32()};
  TypeInt32x1 = BinaryenTypeCreate(_TypeInt32x1, 1);
  BinaryenType _TypeInt32x2[2] = {BinaryenTypeInt32(), BinaryenTypeInt32()};
  TypeInt32x2 = BinaryenTypeCreate(_TypeInt32x2, 2);
  TypeNone = BinaryenTypeNone();

  BinaryenAddFunctionImport(module, "init_malloc", "env", "init_malloc",
                            TypeNone, TypeNone);
  BinaryenAddFunctionImport(module, "printf", "env", "printf", TypeInt32x2,
                            TypeInt32);
  BinaryenAddFunctionImport(module, "log_int", "env", "log_int", TypeInt32x1,
                            TypeNone);

  BinaryenAddFunctionImport(module, "log_string", "env", "log_string",
                            TypeInt32x1, TypeNone);

  BinaryenAddFunctionImport(module, "log_string_n", "env", "log_string_n",
                            TypeInt32x2, TypeNone);
}

BinaryenExpressionRef wasm_log_int(Wasm* wasm, int int32) {
  BinaryenModuleRef module = wasm->module;

  BinaryenExpressionRef operands[] = {make_int32(module, int32)};

  BinaryenExpressionRef log_int =
      BinaryenCall(module, "log_int", operands, 1, TypeNone);

  return log_int;
}

BinaryenExpressionRef wasm_offset(Wasm* wasm, int offset) {
  BinaryenModuleRef module = wasm->module;

  BinaryenExpressionRef wasm_data_end =
      BinaryenGlobalGet(module, "__data_end", TypeInt32);

  BinaryenExpressionRef wasm_offset = make_int32(module, offset);

  BinaryenExpressionRef str_p =
      BinaryenBinary(module, BinaryenAddInt32(), wasm_data_end, wasm_offset);
  return str_p;
}

BinaryenExpressionRef wasm_log_string(Wasm* wasm, int offset) {
  BinaryenModuleRef module = wasm->module;

  BinaryenExpressionRef operands[] = {wasm_offset(wasm, offset)};

  BinaryenExpressionRef log_string =
      BinaryenCall(module, "log_string", operands, 1, TypeNone);

  return log_string;
}

BinaryenExpressionRef wasm_log_string_n(Wasm* wasm, int offset, int n) {
  BinaryenModuleRef module = wasm->module;

  BinaryenExpressionRef operands[] = {wasm_offset(wasm, offset),
                                      make_int32(module, n)};

  BinaryenExpressionRef log_string_n =
      BinaryenCall(module, "log_string_n", operands, 2, TypeNone);

  return log_string_n;
}

BinaryenExpressionRef wasm_printf(Wasm* wasm, int offset) {
  BinaryenModuleRef module = wasm->module;

  BinaryenExpressionRef operands[] = {wasm_offset(wasm, offset),
                                      make_int32(module, 0)};

  BinaryenExpressionRef printf =
      BinaryenCall(module, "printf", operands, 2, TypeNone);

  BinaryenExpressionRef drop = BinaryenDrop(module, printf);
  return drop;
}

void add_test_fn(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  BinaryenType localTypes[] = {BinaryenTypeInt32(), BinaryenTypeInt32()};

  int offset = 5;
  BinaryenExpressionRef printf = wasm_printf(wasm, offset);
  BinaryenExpressionRef log_int = wasm_log_int(wasm, 123);
  BinaryenExpressionRef log_string = wasm_log_string(wasm, offset);
  BinaryenExpressionRef log_string_n = wasm_log_string_n(wasm, offset, 2);

  BinaryenExpressionRef my_value_list[] = {printf, log_int, log_string,
                                           log_string_n};
  BinaryenExpressionRef body =
      BinaryenBlock(module, "my-block", my_value_list,
                    sizeof(my_value_list) / sizeof(BinaryenExpressionRef),
                    BinaryenTypeAuto());

  BinaryenAddFunction(module, "test", TypeNone, TypeInt32x1, localTypes,
                      sizeof(localTypes) / sizeof(BinaryenType), body);
}

void write_string(char* file_name, char* str) {
  FILE* f = fopen(file_name, "wb");
  fputs(str, f);
  fclose(f);
}
void write_binary(char* file_name, void* bin, size_t size) {
  FILE* f = fopen(file_name, "wb");
  fwrite(bin, 1, size, f);
  fclose(f);
}

void write_wat(Wasm* wasm, char* file_name) {
  BinaryenSetColorsEnabled(0);
  char* output = BinaryenModuleAllocateAndWriteText(wasm->module);
  /* printf("%s %d\n", output, _strlen(output)); */
  write_string(file_name, output);
  free(output);
  BinaryenSetColorsEnabled(1);
}

void write_wasm(Wasm* wasm, char* file_name) {
  BinaryenModuleRef module = wasm->module;
  BinaryenModuleAllocateAndWriteResult result =
      BinaryenModuleAllocateAndWrite(module, NULL);

  void* binary = result.binary;
  size_t binaryBytes = result.binaryBytes;
  char* sourceMap = result.sourceMap;
  /* printf("%s \n%zu \n%s\n", binary, binaryBytes, sourceMap); */
  write_binary(file_name, binary, binaryBytes);
  free(binary);
  free(sourceMap);
}

Lval* parse_file(char* file_name) {
  char* str = read_file(file_name);
  if (!str) {
    printf("Could not load file %s", str);
    return NULL;
  }

  int pos = 0;
  scoped Lval* lval_list = lval_read_list(str, &pos, '\0');
  if (lval_list->type == LVAL_ERR) {
    lval_println(lval_list);
    return NULL;
  }
  return lval_list;
}

int compile(char* file_name) {
  info("Compiling %s\n", file_name);

  Lval* lval_list = parse_file(file_name);

  // Wasm
  Wasm* wasm = init_wasm();
  import_runtime(wasm);

  compile_list(wasm, lval_list);
  add_test_fn(wasm);

  strings_data_cat(wasm, "foo3");
  strings_data_cat(wasm, "bar");
  /* printf("strings_data: %s\n", wasm->strings + 4); */

  BinaryenAddFunctionExport(wasm->module, "test", "test");

  // We only have this data _after_ compiling
  /* add_global_section(module); */
  /* add_function_table(module); */
  add_memory_section(wasm);

  /* BinaryenModulePrint(wasm->module); */

  write_wat(wasm, "compiled/lispy.wat");
  write_wasm(wasm, "compiled/lispy.wasm");

  free_wasm(wasm);
  return 1;
}
