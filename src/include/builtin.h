#ifndef __BUILTIN_H_
#define __BUILTIN_H_

#include "wasm.h"
typedef struct wasm_fn WasmFn;

struct wasm_fn {
  char* wasm_fn_name;
  int fn_table_index;
  void (*add_fn)(Wasm*, char*);
  CResult (*compile_builtin_call)(Wasm*, WasmFn, Cell*);
  int data_offset;
};

void register_c_fns();
void import_c_fns(Wasm* wasm);
void register_native_fns();
void assign_offsets_to_builtins();
void make_builtin_module(char* path, char* name);
char* builtin_to_wasm(char* path, char* name);
#endif  // __BUILTIN_H_
