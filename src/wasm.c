#include "wasm.h"

#include <binaryen-c.h>

#include "compile_partial.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "misc_fns.h"
#include "namespace.h"
#include "native.h"
#include "platform.h"
#include "print.h"
#include "printf.h"
#include "state.h"
#include "wasm_util.h"

// Native fns take same args as call and bundle-args fns, ie wval,
// args_block_ptr and args_count.
WasmFn native_fns[] = {
    {"rt_error_too_few_args", -1, add_rt_error_too_few_args_fn, NULL},
    {"rt_error_too_many_args", -1, add_rt_error_too_many_args_fn, NULL},
    {"rt_error_not_a_fn", -1, add_fn_rt_error_not_a_fn, NULL},
    {"copy_and_retain", -1, add_copy_and_retain_fn, NULL},
    {"partial", -1, add_partial_fn, compile_partial_call},
    {"apply", -1, add_apply_fn, NULL},
    {"vector", -1, add_vector_fn, NULL},
    {"set", -1, add_set_fn, NULL},
};

Wasm* init_wasm() {
  Wasm* wasm = calloc(sizeof(Wasm), 1);
  char* runtime_data_end_str = read_file("__data_end");
  int pic = 1;  // position independent code
  int builtin_data_start = (int)_strtol(runtime_data_end_str, NULL, 10);
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
      .__data_end = pic ? 0 : builtin_data_start,
      .__fn_table_end = pic ? 0 : fn_table_end,
      .builtins_data_start = builtin_data_start,
      /* .data_start = (int)_strtol(data_end_str, NULL, 10), */
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
      .cell_offsets = malloc(100 * sizeof(int)),
      .cell_offsets_count = 0,
      .cell_offsets_allocated = 100,
      .id = 1,
      .buf = malloc(1024),
  };

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
  free(wasm->buf);
  free(wasm->context);  // TODO
  // TODO: free string_pool
  free(wasm);
}

void add_memory_section(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  BinaryenAddGlobalImport(module, "__data_end", "env", "__data_end",
                          BinaryenTypeInt32(), 0);
  BinaryenAddMemoryImport(module, "memory", "env", "memory", 0);
  BinaryenAddGlobalImport(module, "data_offset", "env", "data_offset",
                          BinaryenTypeInt32(), 0);
  BinaryenAddGlobalImport(module, "fn_table_offset", "env", "fn_table_offset",
                          BinaryenTypeInt32(), 0);

  const int num_segments = 1;
  const char* segments[1] = {wasm->data};
  BinaryenIndex segmentSizes[] = {wasm->data_offset};

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

CFn c_fns[] = {
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
    {NULL, NULL, "make_lval_wasm_lambda", 4, 1},
    {NULL, NULL, "get_wval_type", 1, 1},
    {NULL, NULL, "get_wval_subtype", 1, 1},
    {NULL, NULL, "get_wval_fn_table_index", 1, 1},
    {NULL, NULL, "get_wval_closure", 1, 1},
    {NULL, NULL, "get_wval_partials", 1, 1},
    {NULL, NULL, "get_wval_partial_count", 1, 1},
    {NULL, NULL, "bundle_rest_args", 3, 0},
    {NULL, NULL, "rewrite_pointers", 3, 0},
    {NULL, NULL, "new_cell", 2, 1},
    {NULL, NULL, "dbg", 1, 0},
    {NULL}};

extern CFn list_c_fns[];
extern CFn math_c_fns[];
extern CFn util_c_fns[];

void add_c_fn_imports(Wasm* wasm, CFn runtime_fns[]) {
  BinaryenModuleRef module = wasm->module;
  int i = 0;
  char* c_fn_name;
  int params_count, results_count;
  do {
    c_fn_name = runtime_fns[i].c_fn_name;
    if (!c_fn_name) break;  // end of list
    params_count = runtime_fns[i].params_count;
    results_count = runtime_fns[i].results_count;
    BinaryenAddFunctionImport(module, c_fn_name, "env", c_fn_name,
                              make_type_int32(params_count),
                              make_type_int32(results_count));
    if (runtime_fns[i].wajure_fn_name)
      state->wajure_to_c_fn_map = alist_prepend(
          state->wajure_to_c_fn_map, runtime_fns[i].wajure_fn_name, c_fn_name);
    i++;
  } while (1);
}

void register_fns(CFn runtime_fns[]) {
  int i = 0;
  char* c_fn_name;
  do {
    c_fn_name = runtime_fns[i].c_fn_name;
    if (!c_fn_name) break;  // end of list
    if (runtime_fns[i].wajure_fn_name)
      state->wajure_to_c_fn_map = alist_prepend(
          state->wajure_to_c_fn_map, runtime_fns[i].wajure_fn_name, c_fn_name);
    i++;
  } while (1);
}

void import_c_fns(Wasm* wasm) {
  add_c_fn_imports(wasm, c_fns);
  add_c_fn_imports(wasm, math_c_fns);
  add_c_fn_imports(wasm, list_c_fns);
  add_c_fn_imports(wasm, util_c_fns);
}

void register_c_fns() {
  register_fns(c_fns);
  register_fns(math_c_fns);
  register_fns(list_c_fns);
  register_fns(util_c_fns);
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
  /* int ptr_len = 10; */
  /* int max_len = _strlen(sym) + ptr_len + _strlen(type_str) + 10; */
  int max_len = 1024;
  char* line = malloc(max_len);
  int offset = lval->data_offset;
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
  // Since we load config->stdlib ("wajure.core") first in nodejs that module
  // will have offset of 0 for data and fns, so we add the call fns to
  // wajure.core and now we can refer to these call fns by index 0-20 throughout
  // all the modules.
  add_call_fns(wasm);             // 0-20
  add_bundle_rest_arg_fns(wasm);  //  21-41

  int fns_count = sizeof(native_fns) / sizeof(*native_fns);

  // 42 - (42 + fns_count)
  for (int i = 0; i < fns_count; i++) {
    native_fns[i].add_fn(wasm, native_fns[i].wasm_fn_name);
    native_fns[i].fn_table_index =
        add_fn_to_table(wasm, native_fns[i].wasm_fn_name);
    char* fn_name = native_fns[i].wasm_fn_name;
    char* type_str = "Function";
    int max_len = 1024;
    char* line = malloc(max_len);
    snprintf(line, max_len, "%s,%s,%d,%d,%d,%d\n", fn_name, type_str, -1,
             native_fns[i].fn_table_index, 1, 1);

    int line_count = _strlen(line);
    wasm->symbol_table =
        realloc(wasm->symbol_table, wasm->symbol_table_count + line_count);
    _strncpy(wasm->symbol_table + wasm->symbol_table_count, line, line_count);
    free(line);
    wasm->symbol_table_count += line_count;
    /* printf("native_fn index: %s %d\n", native_fns[i].wajure_fn_name, */
    /*        native_fns[i].fn_table_index); */
  }
}

void register_native_relay_arrays() {
  // We read the symbol table custom section of stdlib (wajure.core)
  FILE* fp;
  char command[1000];
  char* file_name = ns_to_wasm(config->stdlib);
  sprintf(command, "node custom.js %s call_relay_arrays\n", file_name);
  release(file_name);
  fp = popen(command, "r");

  char fn_name[1000];
  // Line by line
  do {
    char line[1000];
    int ret = fscanf(fp, "%s", line);
    if (ret > 0) {
      strsubst(line, ',', ' ');
      int* data_offset = malloc(sizeof(int));
      sscanf(line, "%s %d", fn_name, data_offset);
      state->native_call_to_relay_array_map = alist_prepend(
          state->native_call_to_relay_array_map, fn_name, data_offset);
      /* printf("Register: %s %d\n", fn_name, *data_offset); */
    } else
      break;
  } while (1);
  fclose(fp);
}

void assign_fn_table_index_to_native_fns(Wasm* wasm) {
  // We read the symbol table custom section of stdlib (wajure.core)
  FILE* fp;
  char command[1000];
  char* file_name = ns_to_wasm(config->stdlib);
  sprintf(command, "node custom.js %s symbol_table\n", file_name);
  release(file_name);
  fp = popen(command, "r");

  char fn_name[1000];
  int data_offset, fn_table_index, param_count, has_rest_arg;
  // Line by line
  do {
    char line[1000];
    int ret = fscanf(fp, "%s", line);
    if (ret > 0) {
      strsubst(line, ',', ' ');
      char type[100];
      sscanf(line, "%s %s %d %d %d %d", fn_name, type, &data_offset,
             &fn_table_index, &param_count, &has_rest_arg);

      // See if it's one of our native fns
      WasmFn* native_fn = (WasmFn*)alist_get(state->wajure_to_native_fn_map,
                                             is_eq_str, fn_name);
      // If so set its fn_table_index
      if (native_fn) {
        /* printf("assigning %d to %s\n", fn_table_index, fn_name); */
        native_fn->fn_table_index = fn_table_index;
      }
    } else
      break;
  } while (1);
  fclose(fp);
}

void register_native_fns() {
  int fns_count = sizeof(native_fns) / sizeof(*native_fns);
  for (int i = 0; i < fns_count; i++) {
    state->wajure_to_native_fn_map =
        alist_prepend(state->wajure_to_native_fn_map,
                      native_fns[i].wasm_fn_name, &native_fns[i]);
  };
}
