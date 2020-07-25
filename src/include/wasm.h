#ifndef __WASM_INIT_H_
#define __WASM_INIT_H_
#include <binaryen-c.h>

#include "ltypes.h"

typedef struct {
  BinaryenModuleRef module;
  char* strings;
  int strings_offset;
  char* lvals;
  int lvals_offset;
  char** fn_names;
  int fns_count;
} Wasm;

Wasm* init_wasm();
void free_wasm(Wasm* wasm);

void import_runtime(Wasm* wasm);
void add_memory_section(Wasm* wasm);
void add_function_table(Wasm* wasm);

#endif  // __WASM_INIT_H_
