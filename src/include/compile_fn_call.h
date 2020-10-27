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

#endif  // __COMPILE_FN_CALL_H_
