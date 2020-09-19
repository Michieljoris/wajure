#include "wasm.h"

#include <binaryen-c.h>

#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "misc_fns.h"
#include "platform.h"
#include "printf.h"
#include "wasm_util.h"

/* char* heap_base_str = read_file("__heap_base"); */

Wasm* init_wasm() {
  Wasm* wasm = calloc(sizeof(Wasm), 1);
  char* data_end_str = read_file("__data_end");
  int pic = 1;  // position independent code
  int data_end = (int)_strtol(data_end_str, NULL, 10);
  int fn_table_end = 0;
  *wasm = (Wasm){
      .module = BinaryenModuleCreate(),
      .data = malloc(4),
      .data_offset = 4,  // we don't want any data at 0 (NULL pointer)
      .fn_names = malloc(1),
      .fns_count = 0,
      .lval_true_offset = 0,
      .lval_false_offset = 0,
      .lval_nil_offset = 0,
      .lval_empty_list_offset = 0,
      .pic = pic,
      .__data_end = pic ? 0 : data_end,
      .__fn_table_end = pic ? 0 : fn_table_end,
      /* .__heap_base = (int)_strtol(heap_base_str, NULL, 10), */
      // reusing interred lval literal numbers for these common
      // numbers (-100 till 100):
      .lval_num_start = -100,
      .lval_num_end = 100,
      .lval_num_offset = calloc(sizeof(CResult), 201),
      .context = malloc(sizeof(Cell)),
      .runtime_check_args_count = 1,
      .symbol_table = malloc(1),
      .symbol_table_count = 0,
      .lval_offsets = malloc(100 * sizeof(int)),
      .lval_offsets_count = 0,
      .lval_offsets_allocated = 100,
      .wval_fn_offsets = malloc(100 * sizeof(int)),
      .wval_fn_offsets_count = 0,
      .wval_fn_offsets_allocated = 100,
      .cell_offsets = malloc(100 * sizeof(int)),
      .cell_offsets_count = 0,
      .cell_offsets_allocated = 100,
  };
  wasm->data[0] = 15;
  wasm->data[1] = 15;
  wasm->data[2] = 15;
  wasm->data[3] = 15;
  for (int i = 0; i < 201; i++) wasm->lval_num_offset[i] = 0;
  Context context = (Context){.msg = "Root context"};
  wasm->context->car = &context;

  BinaryenModuleSetFeatures(wasm->module, BinaryenFeatureMutableGlobals());

  return wasm;
}

void free_wasm(Wasm* wasm) {
  BinaryenModuleDispose(wasm->module);
  free(wasm->data);
  free(wasm->lval_num_offset);
  while (wasm->fns_count--) free(wasm->fn_names[wasm->fns_count]);
  free(wasm->fn_names);
  free(wasm->context);  // TODO
  // TODO: free string_pool
  free(wasm);
}

void add_memory_section(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  BinaryenAddGlobalImport(module, "__data_end", "env", "__data_end",
                          BinaryenTypeInt32(), 0);
  BinaryenAddMemoryImport(module, "memory", "env", "memory", 0);
  BinaryenAddGlobalImport(module, "stack_pointer", "env", "stack_pointer",
                          BinaryenTypeInt32(), 1);
  BinaryenAddGlobalImport(module, "data_offset", "env", "data_offset",
                          BinaryenTypeInt32(), 0);
  BinaryenAddGlobalImport(module, "fn_table_offset", "env", "fn_table_offset",
                          BinaryenTypeInt32(), 0);

  /* BinaryenAddGlobal(module, "stack_pointer", BinaryenTypeInt32(), 1, */
  /*                   make_int32(module, wasm->__heap_base)); */
  BinaryenAddGlobalExport(module, "stack_pointer", "stack_pointer");
  const int num_segments = 1;
  const char* segments[1] = {wasm->data};
  BinaryenIndex segmentSizes[] = {wasm->data_offset};

  /* BinaryenExpressionRef __data_end = */
  /*     BinaryenGlobalGet(module, "__data_end", BinaryenTypeInt32()); */
  Ber data_offset =
      wasm->pic ? BinaryenGlobalGet(module, "data_offset", BinaryenTypeInt32())
                : make_int32(module, wasm->__data_end);
  BinaryenExpressionRef segmentOffsets[1] = {data_offset};
  int8_t segmentPassive[] = {0};

  int initial_mem_size = 2;
  /* int max_mem_size = 32767; */
  int max_mem_size = 65536;
  int shared = 0;
  BinaryenSetMemory(module, initial_mem_size, max_mem_size, "mem", segments,
                    segmentPassive, segmentOffsets, segmentSizes, num_segments,
                    shared);
}

void add_function_table(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  BinaryenAddTableImport(module, "fn_table", "env", "fn_table");
  int initial = wasm->fns_count;
  int maximum = wasm->fns_count;
  initial = 100 * 1000;
  maximum = 1000000;

  const char** funcNames = (const char**)wasm->fn_names;

  for (int i = 0; i < wasm->fns_count; i++) {
    BinaryenAddFunctionExport(wasm->module, funcNames[i], funcNames[i]);
  };
  int numFuncNames = wasm->fns_count;

  Ber fn_table_offset = wasm->pic ? BinaryenGlobalGet(module, "fn_table_offset",
                                                      BinaryenTypeInt32())
                                  : make_int32(module, wasm->__fn_table_end);
  BinaryenSetFunctionTable(module, initial, maximum, funcNames, numFuncNames,
                           fn_table_offset);
}

typedef struct {
  char* func_name;
  int params_count;
  int results_count;
} RuntimeFunction;

LispyFn runtime_fns[] = {
    // from js
    {NULL, NULL, "log_int", 1, 0},
    {NULL, NULL, "log_string", 1, 0},
    {NULL, NULL, "log_string_n", 2, 0},
    {NULL, NULL, "runtime_error", 2, 0},
    // print
    {NULL, NULL, "lval_print", 1, 0},
    {NULL, NULL, "lval_println", 1, 0},
    // printf
    {NULL, NULL, "printf_", 2, 1},
    // lval
    {NULL, NULL, "make_lval_num", 1, 1},
    {NULL, NULL, "make_lval_nil", 0, 1},
    {NULL, NULL, "make_lval_true", 0, 1},
    {NULL, NULL, "make_lval_false", 0, 1},
    {NULL, NULL, "make_lval_str", 1, 1},
    {NULL, NULL, "make_lval_list", 0, 1},
    {NULL, NULL, "new_lval_list", 1, 1},
    {NULL, NULL, "make_lval_sym", 1, 1},
    // lispy_mempool
    {NULL, NULL, "lalloc_size", 1, 1},
    {NULL, NULL, "lalloc_type", 1, 1},
    // refcount
    {NULL, NULL, "retain", 1, 1},
    {NULL, NULL, "release", 1, 0},

    // list
    {NULL, NULL, "list_cons", 2, 1},
    // lib
    {NULL, NULL, "_strcpy", 2, 1},
    {NULL, NULL, "print_slot_size", 0, 0},
    // runtime
    {NULL, NULL, "wval_print", 1, 0},
    {NULL, NULL, "make_lval_wasm_lambda", 6, 1},
    {NULL, NULL, "get_wval_type", 1, 1},
    {NULL, NULL, "get_wval_subtype", 1, 1},
    {NULL, NULL, "get_wval_fn_table_index", 1, 1},
    {NULL, NULL, "get_wval_param_count", 1, 1},
    {NULL, NULL, "get_wval_has_rest_arg", 1, 1},
    {NULL, NULL, "get_wval_closure", 1, 1},
    {NULL, NULL, "get_wval_partials", 1, 1},
    {NULL, NULL, "get_wval_partial_count", 1, 1},
    {NULL, NULL, "init_rest_args", 2, 0},
    {NULL, NULL, "check_args_count", 3, 1},
    {NULL, NULL, "rewrite_pointers", 3, 0},
    {NULL}};

extern LispyFn list_builtin_fns[];
extern LispyFn math_builtin_fns[];
extern LispyFn util_builtin_fns[];

void runtime_add_fns(Wasm* wasm, LispyFn lispy_fns[]) {
  BinaryenModuleRef module = wasm->module;
  int i = 0;
  char* c_fn_name;
  int params_count, results_count;
  do {
    c_fn_name = lispy_fns[i].c_fn_name;
    if (!c_fn_name) break;
    params_count = lispy_fns[i].params_count;
    results_count = lispy_fns[i].results_count;
    BinaryenAddFunctionImport(module, c_fn_name, "env", c_fn_name,
                              make_type_int32(params_count),
                              make_type_int32(results_count));
    if (lispy_fns[i].lispy_fn_name)
      wasm->lispy_to_c_fn_map = alist_prepend(
          wasm->lispy_to_c_fn_map, lispy_fns[i].lispy_fn_name, c_fn_name);
    i++;
  } while (c_fn_name);
}

void import_runtime(Wasm* wasm) {
  runtime_add_fns(wasm, runtime_fns);
  runtime_add_fns(wasm, math_builtin_fns);
  runtime_add_fns(wasm, list_builtin_fns);
  runtime_add_fns(wasm, util_builtin_fns);
}

CResult quit(Wasm* wasm, char* fmt, ...) {
  if (fmt) {
    va_list va;
    va_start(va, fmt);
    char* str = lalloc_size(512);
    vsnprintf(str, 511, fmt, va);
    str = lrealloc(str, _strlen(str) + 1);
    va_end(va);
    printf("%s\n", str);
  }
  printf("Context: ");
  print_wasm_context(wasm);
  printf("NOTE: Lispy compilation ended abnormally.\n");
  free(wasm);
  exit(1);
  return cnull();
}

void add_to_symbol_table(Wasm* wasm, char* sym, Lval* lval) {
  /* printf("-- ADD TO SYMBOL TABLE\n"); */
  char* type_str = lval_type_to_name(lval);
  int ptr_len = 10;
  int max_len = _strlen(sym) + ptr_len + _strlen(type_str) + 10;
  char* line = malloc(max_len);
  /* int data_end = wasm->__data_end; */
  /* int offset = lval->wval_ptr - data_end; */
  int offset = lval->wval_ptr;
  /* printf("OFFSET: %d\n", offset); */
  if (lval->type == LVAL_FUNCTION)
    snprintf(line, max_len, "%s,%s,%d,%d,%d,%d\n", sym, type_str, offset,
             lval->offset, lval->param_count, lval->rest_arg_index);
  else
    snprintf(line, max_len, "%s,%s,%d\n", sym, type_str, offset);
  int line_count = _strlen(line);
  /* printf("line: %d %d %d %d %s", lval->wval_ptr, data_end, */
  /*        wasm->symbol_table_count, line_count, line); */
  wasm->symbol_table =
      realloc(wasm->symbol_table, wasm->symbol_table_count + line_count);
  _strncpy(wasm->symbol_table + wasm->symbol_table_count, line, line_count);
  free(line);
  wasm->symbol_table_count += line_count;
  /* printf("%.*s", wasm->symbol_table_count, wasm->symbol_table); */
}
