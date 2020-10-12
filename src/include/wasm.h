#ifndef __WASM_INIT_H_
#define __WASM_INIT_H_
#include <binaryen-c.h>

#include "ltypes.h"

#define Ber BinaryenExpressionRef

typedef struct {
  Ber ber;
  int data_offset;
  int is_fn_call;
  Lval* lval;
} CResult;

typedef struct {
  BinaryenModuleRef module;
  char* data;
  int data_offset;
  char** fn_names;
  int fns_count;
  Lenv* env;
  int pic;  // position independent code
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

  int validate_fn_at_rt;
  Map deps;
  int* fn_relay_table_offsets;
  int* fn_relay_table_offsets_has_rest_arg;
  int id;
  char* buf;
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

typedef struct native_fn NativeFn;

struct native_fn {
  char* wajure_fn_name;
  int fn_table_index;
  void (*add_fn)(Wasm*);
  CResult (*compile_fn_call)(Wasm*, NativeFn, Cell*);
};

void add_native_fns(Wasm* wasm);
void register_wajure_native_fns(Wasm* wasm);

#endif  // __WASM_INIT_H_
