#ifndef __WASM_INIT_H_
#define __WASM_INIT_H_
#include <binaryen-c.h>

typedef struct {
  BinaryenModuleRef module;
  char* strings;
  int strings_offset;
} Wasm;

extern BinaryenType TypeInt32;
extern BinaryenType TypeInt32x1;
extern BinaryenType TypeInt32x2;
extern BinaryenType TypeNone;

Wasm* init_wasm();
void free_wasm(Wasm* wasm);

void import_runtime(Wasm* wasm);
void add_memory_section(Wasm* wasm);

#endif  // __WASM_INIT_H_
