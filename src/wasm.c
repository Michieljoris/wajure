#include "wasm.h"

#include <binaryen-c.h>

#include "lib.h"
#include "misc_fns.h"
#include "platform.h"
#include "wasm_util.h"

Wasm* init_wasm() {
  Wasm* wasm = malloc(sizeof(Wasm));
  char* data_end_str = read_file("__data_end");
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
                 // no need to intern lval literal numbers for these common
                 // numbers (-100 till 100):
                 .lval_num_start = -100,
                 .lval_num_end = 100,
                 .lval_num_offset = calloc(sizeof(BinaryenExpressionRef), 201)};
  return wasm;
}

void free_wasm(Wasm* wasm) {
  BinaryenModuleDispose(wasm->module);
  release_env(wasm->env);
  free(wasm->strings);
  free(wasm->lval_num_offset);
  free(wasm);
}

void add_memory_section(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  /* BinaryenAddGlobalImport(module, "__data_end", "env", "__data_end", */
  /*                         BinaryenTypeInt32(), 0); */
  BinaryenAddMemoryImport(module, "memory", "env", "memory", 0);
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

RuntimeFunction runtime_functions[] = {{"printf_", 2, 1},
                                       {"log_int", 1, 0},
                                       {"log_string", 1, 0},
                                       {"log_string_n", 2, 0},
                                       {"lval_print", 1, 0},
                                       {"lval_println", 1, 0},
                                       // lval
                                       {"make_lval_num", 1, 1},
                                       {"make_lval_nil", 0, 1},
                                       {"make_lval_true", 0, 1},
                                       {"make_lval_false", 0, 1},
                                       {"make_lval_str", 1, 1},
                                       {"make_lval_list", 0, 1},
                                       {"new_lval_list", 1, 1},
                                       {"make_lval_sym", 1, 1},
                                       // lispy_mempool
                                       {"lalloc_size", 1, 1},
                                       // math_fns
                                       /* {"add_fn", 2, 1}, */
                                       /* {"sub_fn", 2, 1}, */
                                       /* {"mul_fn", 2, 1}, */
                                       /* {"div_fn", 2, 1}, */
                                       /* {"gt_fn", 2, 1}, */
                                       /* {"lt_fn", 2, 1}, */
                                       /* {"gte_fn", 2, 1}, */
                                       /* {"lte_fn", 2, 1}, */
                                       /* {"eq_fn", 2, 1}, */
                                       /* {"not_eq_fn", 2, 1}, */
                                       // misc_fns
                                       {"print_fn", 2, 1},
                                       {"boolean_fn", 2, 1},
                                       // list
                                       {"list_cons", 2, 1},
                                       // lib
                                       {"_strcpy", 2, 1},
                                       {"print_slot_size", 0, 0},
                                       {NULL}};

void import_runtime(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;
  int i = 0;
  char* func_name;
  int params_count, results_count;
  do {
    func_name = runtime_functions[i].func_name;
    if (!func_name) break;
    params_count = runtime_functions[i].params_count;
    results_count = runtime_functions[i].results_count;
    BinaryenAddFunctionImport(module, func_name, "env", func_name,
                              make_type_int32(params_count),
                              make_type_int32(results_count));
    i++;
  } while (func_name);
}
