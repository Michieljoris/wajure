#include "wasm.h"

#include <binaryen-c.h>

#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "misc_fns.h"
#include "platform.h"
#include "wasm_util.h"

Wasm* init_wasm() {
  Wasm* wasm = malloc(sizeof(Wasm));
  char* data_end_str = read_file("__data_end");
  char* heap_base_str = read_file("__heap_base");
  *wasm = (Wasm){.module = BinaryenModuleCreate(),
                 .strings = malloc(1),
                 .strings_offset = 0,
                 .fn_names = malloc(1),
                 .fns_count = 0,
                 .lval_true_offset = NULL,
                 .lval_false_offset = NULL,
                 .lval_nil_offset = NULL,
                 .lval_empty_list_offset = NULL,
                 .__data_end = (int)_strtol(data_end_str, NULL, 10),
                 .__heap_base = (int)_strtol(heap_base_str, NULL, 10),
                 // no need to intern lval literal numbers for these common
                 // numbers (-100 till 100):
                 .lval_num_start = -100,
                 .lval_num_end = 100,
                 .lval_num_offset = calloc(sizeof(BinaryenExpressionRef), 201),
                 .context = malloc(sizeof(Cell)),
                 .id = 1,
                 .is_fn_call = 0,
                 .runtime_check_args_count = 1};
  Context context = (Context){.msg = "Root context"};
  wasm->context->car = &context;

  BinaryenModuleSetFeatures(wasm->module, BinaryenFeatureMutableGlobals());

  return wasm;
}

void free_wasm(Wasm* wasm) {
  BinaryenModuleDispose(wasm->module);
  release_env(wasm->env);
  free(wasm->strings);
  free(wasm->lval_num_offset);
  while (wasm->fns_count--) free(wasm->fn_names[wasm->fns_count]);
  free(wasm->fn_names);
  free(wasm->context);  // TODO
  free(wasm);
}

void add_memory_section(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  BinaryenAddGlobalImport(module, "__data_end", "env", "__data_end",
                          BinaryenTypeInt32(), 0);
  BinaryenAddMemoryImport(module, "memory", "env", "memory", 0);
  BinaryenAddGlobalImport(module, "stack_pointer", "env", "stack_pointer",
                          BinaryenTypeInt32(), 1);

  /* BinaryenAddGlobal(module, "stack_pointer", BinaryenTypeInt32(), 1, */
  /*                   make_int32(module, wasm->__heap_base)); */
  BinaryenAddGlobalExport(module, "stack_pointer", "stack_pointer");
  const int num_segments = 1;
  const char* segments[1] = {wasm->strings};
  BinaryenIndex segmentSizes[] = {wasm->strings_offset};

  /* BinaryenExpressionRef __data_end = */
  /*     BinaryenGlobalGet(module, "__data_end", BinaryenTypeInt32()); */
  /* BinaryenExpressionRef segmentOffsets[1] = {__data_end}; */
  BinaryenExpressionRef segmentOffsets[1] = {
      make_int32(module, wasm->__data_end)};
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
  /* BinaryenAddTableImport(module, "table", "env", "table"); */
  int initial = wasm->fns_count;
  int maximum = wasm->fns_count;

  const char** funcNames = (const char**)wasm->fn_names;
  int numFuncNames = wasm->fns_count;
  BinaryenExpressionRef offset = make_int32(module, 0);
  BinaryenSetFunctionTable(module, initial, maximum, funcNames, numFuncNames,
                           offset);
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
    {NULL, NULL, "runtime_error", 1, 0},
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
    {NULL, NULL, "init_rest_args", 2, 0},
    {NULL, NULL, "check_args_count", 3, 1},
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

void* quit(Wasm* wasm, char* fmt, ...) {
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
}
