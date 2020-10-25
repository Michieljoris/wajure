#include "wasm.h"

#include <binaryen-c.h>

#include "builtin.h"
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
  /*   printf("%s\n", funcNames[i]); */
  /*   /\* BinaryenAddFunctionExport(wasm->module, funcNames[i], funcNames[i]);
   * *\/ */
  /* }; */
  int numFuncNames = wasm->fns_count;

  Ber fn_table_offset = wasm->pic ? BinaryenGlobalGet(module, "fn_table_offset",
                                                      BinaryenTypeInt32())
                                  : make_int32(module, wasm->__fn_table_end);
  BinaryenSetFunctionTable(module, initial, maximum, funcNames, numFuncNames,
                           fn_table_offset);
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

void write_symbol_table_line(Wasm* wasm, int type, char* fn_name,
                             int data_offset, int fn_table_index,
                             int param_count, int has_rest_arg) {
  char* type_str = lval_type_constant_to_name(type);
  int len = _strlen(fn_name) + _strlen(type_str) + number_len(data_offset) +
            number_len(fn_table_index) + number_len(param_count) +
            number_len(has_rest_arg) + 6 + 1;
  char* line = malloc(len);
  sprintf(line, "%s,%s,%d,%d,%d,%d\n", fn_name, type_str, data_offset,
          fn_table_index, param_count, has_rest_arg);

  int line_count = _strlen(line);
  wasm->symbol_table =
      realloc(wasm->symbol_table, wasm->symbol_table_count + line_count);
  _strncpy(wasm->symbol_table + wasm->symbol_table_count, line, line_count);
  free(line);
  wasm->symbol_table_count += line_count;
}

void add_to_symbol_table(Wasm* wasm, char* sym, Lval* lval) {
  int data_offset = lval->data_offset;
  int fn_table_index = -1;
  int param_count = -1;
  int has_rest_arg = -1;
  if (lval->type == LVAL_FUNCTION) {
    fn_table_index = lval->cfn ? lval->cfn->offset : lval->offset;  // partials
    /* param_count = lval->param_count; */
    /* has_rest_arg = lval->rest_arg_index; */
    param_count = -1;   // unused
    has_rest_arg = -1;  // unused
  }
  write_symbol_table_line(wasm, lval->type, sym, data_offset, fn_table_index,
                          param_count, has_rest_arg);
}
