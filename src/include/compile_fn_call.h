#ifndef __COMPILE_FN_CALL_H_
#define __COMPILE_FN_CALL_H_

#include "ltypes.h"
#include "wasm.h"

union FnRef {
  Ber ber;
  char* fn_name;
  char* global_name;
  int fn_table_index;
};

enum { BER, FN_NAME, INDIRECT_EXTERNAL, INDIRECT_LOCAL };

void add_call_fns(Wasm* wasm);
void add_validate_fn_fn(Wasm* wasm);
void add_partial_fn(Wasm* wasm);

void add_apply_fn(Wasm* wasm);
CResult compile_application(Wasm* wasm, Lval* lval_list);

CResult apply(Wasm* wasm, int fn_ref_type, union FnRef fn_ref, Lval* lval_fun,
              Cell* args);

#endif  // __COMPILE_FN_CALL_H_
