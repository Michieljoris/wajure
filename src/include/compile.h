#ifndef __COMPILE_H_
#define __COMPILE_H_

#include "wasm.h"

// Runtime error codes
enum { RT_WRONG_NUMBER_OF_ARGS, RT_NOT_A_FN };

CResult compile_list(Wasm* wasm, Cell* head);
CResult compile_do_list(Wasm* wasm, Lval* lval_list, Ber init);
void compile(Namespace* ns);

CResult lval_compile(Wasm* wasm, Lval* lval);

CResult wasm_process_args(Wasm* wasm, int param_count, int rest_arg_index);

CResult compile_lval_ref(Wasm* wasm, Lval* lval_symbol, Lval* lval_ref);

CResult compile_special_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args);
CResult compile_sys_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args);
CResult compile_native_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args);
void add_deps_custom_section(Wasm* wasm, char* custom_section_name, Cell* deps);

int compile_main();
#endif  // __COMPILE_H_
