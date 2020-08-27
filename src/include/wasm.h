#ifndef __WASM_INIT_H_
#define __WASM_INIT_H_
#include <binaryen-c.h>

#include "ltypes.h"

#define Ber BinaryenExpressionRef

typedef struct {
  Ber ber;
  int wasm_ptr;
  int is_fn_call;
} CResult;

typedef struct {
  BinaryenModuleRef module;
  char* data;
  int data_offset;
  char** fn_names;
  int fns_count;
  Lenv* env;
  int __data_end;
  int __heap_base;
  CResult lval_true_offset;
  CResult lval_false_offset;
  CResult lval_nil_offset;
  CResult lval_empty_list_offset;
  int lval_num_start;
  int lval_num_end;
  CResult* lval_num_offset;
  Cell* lispy_to_c_fn_map;
  Cell* fn_to_offset_map;
  Cell* context;
  // whether to check at runtime if correct number of args is passed into
  // function
  int runtime_check_args_count;
} Wasm;

Wasm* init_wasm();
void free_wasm(Wasm* wasm);

void import_runtime(Wasm* wasm);
void add_memory_section(Wasm* wasm);
void add_function_table(Wasm* wasm);

CResult quit(Wasm* wasm, char* fmt, ...);

#endif  // __WASM_INIT_H_
