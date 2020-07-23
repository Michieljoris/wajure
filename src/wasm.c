#include "wasm.h"

#include <binaryen-c.h>

#include "platform.h"
#include "wasm_util.h"

Wasm* init_wasm() {
  Wasm* wasm = malloc(sizeof(Wasm));
  *wasm = (Wasm){.module = BinaryenModuleCreate(),
                 .strings = malloc(1),
                 .strings_offset = 0,
                 .fn_names = malloc(1),
                 .fns_count = 0};
  return wasm;
}

void free_wasm(Wasm* wasm) {
  BinaryenModuleDispose(wasm->module);
  free(wasm->strings);
  free(wasm);
}

BinaryenType TypeInt32;
BinaryenType TypeInt32x1;
BinaryenType TypeInt32x2;
BinaryenType TypeNone;

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
