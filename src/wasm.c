#include "wasm.h"

#include <binaryen-c.h>

#include "compile_partial.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "misc_fns.h"
#include "native.h"
#include "platform.h"
#include "print.h"
#include "printf.h"
#include "wasm_util.h"

/* char* heap_base_str = read_file("__heap_base"); */

// TODO: set fn_table_indices dynamically!!!!
// Native fns take same args as call and bundle-args fns, ie wval,
// args_block_ptr and args_count.
NativeFn native_fns[] = {
    {"rt_error_too_few_args", 42, add_rt_error_too_few_args_fn, NULL},
    {"rt_error_too_many_args", 43, add_rt_error_too_many_args_fn, NULL},
    {"copy_and_retain", 44, add_copy_and_retain_fn, NULL},
    {"partial", 45, add_partial_fn, compile_partial_call},
    {"apply", 46, add_apply_fn, compile_partial_call},
};

Wasm* init_wasm() {
  Wasm* wasm = calloc(sizeof(Wasm), 1);
  char* data_end_str = read_file("__data_end");
  int pic = 1;  // position independent code
  int data_end = (int)_strtol(data_end_str, NULL, 10);
  int fn_table_end = 0;
  int native_fns_count = sizeof(native_fns) / sizeof(*native_fns);
  int fn_relay_table_offsets_count = (MAX_FN_PARAMS + 1) * 2 + native_fns_count;
  *wasm =
      (Wasm){.module = BinaryenModuleCreate(),
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
             .validate_fn_at_rt = 1,
             .symbol_table = malloc(1),
             .symbol_table_count = 0,
             .lval_offsets = malloc(100 * sizeof(int)),
             .lval_offsets_count = 0,
             .lval_offsets_allocated = 100,
             /* .wval_fn_offsets = malloc(100 * sizeof(int)), */
             /* .wval_fn_offsets_count = 0, */
             /* .wval_fn_offsets_allocated = 100, */
             .cell_offsets = malloc(100 * sizeof(int)),
             .cell_offsets_count = 0,
             .cell_offsets_allocated = 100,
             .fn_relay_table_offsets =
                 calloc(fn_relay_table_offsets_count, sizeof(int))};

  wasm->data[0] = 15;
  wasm->data[1] = 15;
  wasm->data[2] = 15;
  wasm->data[3] = 15;
  for (int i = 0; i < 201; i++) wasm->lval_num_offset[i] = 0;
  Context context = (Context){.msg = "Root context"};
  wasm->context->car = &context;

  BinaryenModuleSetFeatures(wasm->module, BinaryenFeatureBulkMemory() |
                                              BinaryenFeatureMutableGlobals());

  return wasm;
}

void free_wasm(Wasm* wasm) {
  BinaryenModuleDispose(wasm->module);
  free(wasm->data);
  free(wasm->lval_num_offset);
  while (wasm->fns_count--) free(wasm->fn_names[wasm->fns_count]);
  free(wasm->fn_names);
  free(wasm->fn_relay_table_offsets);
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

  // Exports all fns
  /* for (int i = 0; i < wasm->fns_count; i++) { */
  /*   BinaryenAddFunctionExport(wasm->module, funcNames[i], funcNames[i]); */
  /* }; */
  int numFuncNames = wasm->fns_count;

  Ber fn_table_offset = wasm->pic ? BinaryenGlobalGet(module, "fn_table_offset",
                                                      BinaryenTypeInt32())
                                  : make_int32(module, wasm->__fn_table_end);
  BinaryenSetFunctionTable(module, initial, maximum, funcNames, numFuncNames,
                           fn_table_offset);
}

RuntimeFn runtime_fns[] = {
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
    {NULL, NULL, "new_lval_vector", 1, 1},
    // lispy_mempool
    {NULL, NULL, "lalloc_size", 1, 1},
    {NULL, NULL, "lalloc_type", 1, 1},
    // refcount
    {NULL, NULL, "retain", 1, 1},
    {NULL, NULL, "release", 1, 0},

    // list
    {NULL, NULL, "prefix_list", 2, 1},

    // lib
    {NULL, NULL, "_strcpy", 2, 1},
    {NULL, NULL, "print_slot_size", 0, 0},
    // runtime
    {NULL, NULL, "wval_print", 1, 0},
    {NULL, NULL, "make_lval_wasm_lambda", 5, 1},
    {NULL, NULL, "get_wval_type", 1, 1},
    {NULL, NULL, "get_wval_subtype", 1, 1},
    {NULL, NULL, "get_wval_fn_table_index", 1, 1},
    {NULL, NULL, "get_wval_closure", 1, 1},
    {NULL, NULL, "get_wval_partials", 1, 1},
    {NULL, NULL, "get_wval_partial_count", 1, 1},
    {NULL, NULL, "get_wval_fn_call_relay_array", 1, 1},
    {NULL, NULL, "bundle_rest_args", 3, 0},
    {NULL, NULL, "rewrite_pointers", 3, 0},
    {NULL, NULL, "new_cell", 2, 1},
    {NULL, NULL, "dbg", 1, 0},
    {NULL}};

extern RuntimeFn list_builtin_fns[];
extern RuntimeFn math_builtin_fns[];
extern RuntimeFn util_builtin_fns[];

void runtime_add_fns(Wasm* wasm, RuntimeFn lispy_fns[]) {
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
    if (lispy_fns[i].wajure_fn_name)
      wasm->wajure_to_c_fn_map = alist_prepend(
          wasm->wajure_to_c_fn_map, lispy_fns[i].wajure_fn_name, c_fn_name);
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
  int offset = lval->wval_ptr;
  if (lval->type == LVAL_FUNCTION) {
    int fn_table_index =
        lval->cfn ? lval->cfn->offset : lval->offset;  // partials
    snprintf(line, max_len, "%s,%s,%d,%d,%d,%d\n", sym, type_str, offset,
             fn_table_index, lval->param_count, lval->rest_arg_index);
  } else
    snprintf(line, max_len, "%s,%s,%d\n", sym, type_str, offset);
  int line_count = _strlen(line);
  wasm->symbol_table =
      realloc(wasm->symbol_table, wasm->symbol_table_count + line_count);
  _strncpy(wasm->symbol_table + wasm->symbol_table_count, line, line_count);
  free(line);
  wasm->symbol_table_count += line_count;
}

void add_native_fns(Wasm* wasm) {
  // Since we load config->main ("main") first in nodejs that module will have
  // offset of 0 for data and fns, so we add the call fns to main and now we
  // can refer to these call fns by index 0-20 throughout all the modules.
  add_call_fns(wasm);             // 0-20
  add_bundle_rest_arg_fns(wasm);  //  21-41

  int fns_count = sizeof(native_fns) / sizeof(*native_fns);

  // We sort so we're sure the constants match the fn table index.
  /* qsort(native_fns, fns_count, sizeof(*native_fns), comp); */
  for (int i = 0; i < fns_count; i++) {
    native_fns[i].add_fn(wasm);
    native_fns[i].fn_table_index =
        add_fn_to_table(wasm, native_fns[i].wajure_fn_name);
    /* printf("native_fn index: %s %d\n", native_fns[i].wajure_fn_name, */
    /*        native_fns[i].fn_table_index); */
  }
}

void register_wajure_native_fns(Wasm* wasm) {
  int fns_count = sizeof(native_fns) / sizeof(*native_fns);
  for (int i = 0; i < fns_count; i++) {
    wasm->wajure_to_native_fn_map =
        alist_prepend(wasm->wajure_to_native_fn_map,
                      native_fns[i].wajure_fn_name, &native_fns[i]);
  };
}
