#ifndef __WASM_INIT_H_
#define __WASM_INIT_H_
#include <binaryen-c.h>

#include "ltypes.h"

typedef struct {
  BinaryenModuleRef module;
  char* strings;
  int strings_offset;
  char** fn_names;
  int fns_count;
  Lenv* env;
  int __data_end;
  BinaryenExpressionRef lval_true_offset;
  BinaryenExpressionRef lval_false_offset;
  BinaryenExpressionRef lval_nil_offset;
  int lval_num_start;
  int lval_num_end;
  BinaryenExpressionRef* lval_num_offset;

} Wasm;

Wasm* init_wasm();
void free_wasm(Wasm* wasm);

void import_runtime(Wasm* wasm);
void add_memory_section(Wasm* wasm);
void add_function_table(Wasm* wasm);

#endif  // __WASM_INIT_H_
