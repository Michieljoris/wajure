#ifndef __WASM_INIT_H_
#define __WASM_INIT_H_
#include <binaryen-c.h>

#include "ltypes.h"

#define VALIDATE_FN_INDEX 21
#define PARTIAL_FN_INDEX 22
#define APPLY_FN_INDEX 23
#define NATIVE_INDEX_COUNT 24

#define Ber BinaryenExpressionRef

typedef struct {
  Ber ber;
  int wasm_ptr;
  int is_fn_call;
  int is_external;
  Lval* lval;
} CResult;

typedef struct {
  BinaryenModuleRef module;
  char* data;
  int data_offset;
  char** fn_names;
  int fns_count;
  Lenv* env;
  int pic;
  int __data_end;
  int __fn_table_end;
  /* int __heap_base; */
  int lval_true_offset;
  int lval_false_offset;
  int lval_nil_offset;
  int lval_empty_list_offset;
  int lval_num_start;
  int lval_num_end;
  int* lval_num_offset;
  Cell* wajure_to_c_fn_map;
  Cell* wajure_to_native_fn_map;
  Cell* string_pool;
  Cell* lval_str_pool;
  Cell* lval_symbol_pool;
  Cell* lval_keyword_pool;
  char* symbol_table;
  int symbol_table_count;
  Cell* context;
  // whether to check at runtime if correct number of args is passed into
  // function
  int* lval_offsets;
  int lval_offsets_count;
  int lval_offsets_allocated;
  int* cell_offsets;
  int cell_offsets_count;
  int cell_offsets_allocated;
  int* wval_fn_offsets;
  int wval_fn_offsets_count;
  int wval_fn_offsets_allocated;

  int validate_fn_at_rt;
  Map deps;
} Wasm;

typedef struct {
  Lenv* closure;
  int closure_count;
} FunctionData;

Wasm* init_wasm();
void free_wasm(Wasm* wasm);

void import_runtime(Wasm* wasm);
void add_memory_section(Wasm* wasm);
void add_function_table(Wasm* wasm);

CResult quit(Wasm* wasm, char* fmt, ...);
void add_to_symbol_table(Wasm* wasm, char* sym, Lval* lval);

typedef struct {
  char* wajure_fn_name;
  int fn_table_index;
  int params_count;
  int has_rest_arg;
  int rest_arg_index;
} NativeFn;

void register_native_fns(Wasm* wasm);

#endif  // __WASM_INIT_H_
