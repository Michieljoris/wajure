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

CResult compile_application(Wasm* wasm, Lval* lval_list);

CResult apply(Wasm* wasm, int fn_ref_type, union FnRef fn_ref, Lval* lval_fun,
              Cell* args);

#endif  // __COMPILE_FN_CALL_H_
