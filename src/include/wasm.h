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
  char* strings;
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
  // Every lval is either the result of a fn/lambda call, special form or a
  // retrieving of interred values or previously calculated dynamic values. This
  // flag keeps track of what we just put on the wasm stack is the result of
  // retrieving of a value, or the result of wasn fn call or special form (in a
  // wasm block). We need to keep track of this because we want release all
  // calculated values after they've been passed to another fn, eg in (f (+ 1 2)
  // some-var 123) we want to release the result of (+ 1 2) after f returns, but
  // not some-var and 123.
  //
  // Similarly at the end of a do/let block or fn body we want release all
  // values that were the result of a fn call eg: in (do 123 some-var (+ 1 2) 1)
  // we want to release (+ 1 2) and retain 1. In (do 123 (+ 1 2)) we want to
  // retain (+ 1 2). In (do 123 (+ 1 2) some-var) we want to release (+ 1 2) and
  // retain some-var.
  //
  // In (let [x 1 y (+ 1 x) some-var (+ 1 x)] x some-var) we want to retain
  // some-var, but also release also all bindings that are result of fn calls
  // (so y and some-var)
  //
  // So after any lval_compile we check whether this flag is set. If so we know
  // that we just either made a fn call or we have the result of the execution
  // of a special form on the wasm stack.
  int is_fn_call;
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
