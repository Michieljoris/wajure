#ifndef __WASM_UTIL_H_
#define __WASM_UTIL_H_

#include <binaryen-c.h>

#include "ltypes.h"
#include "platform.h"
#include "wasm.h"

BinaryenExpressionRef make_int32(BinaryenModuleRef module, int x);
void write_wat(Wasm* wasm, char* file_name);

void write_wasm(Wasm* wasm, char* file_name);
void add_test_fn(Wasm* wasm);

int add_string_to_data(Wasm* wasm, char* str);

int add_bytes_to_data(Wasm* wasm, char* str, int len);
int add_fn_to_table(Wasm* wasm, char* fn_name);

BinaryenExpressionRef wasm_offset(Wasm* wasm, int offset);

BinaryenExpressionRef wasm_log_int(Wasm* wasm, int int32);

Lenv* interprete_file(char* file_name);

void release_env(Lenv* env);

BinaryenExpressionRef wasm_printf(Wasm* wasm, int offset);

BinaryenType* make_type_int32_array(int count);

BinaryenType make_type_int32(int count);

BinaryenExpressionRef wasm_offset(Wasm* wasm, int offset);

BinaryenExpressionRef make_lval_literal(Wasm* wasm, Lval* lval);

Wasm* enter_context(Wasm* wasm);

void leave_context(Wasm* wasm);

#define compile_context __attribute__((__cleanup__(_leave_context)))
void _leave_context(void* data);

#define NEW_CONTEXT_LVAL(_msg, _lval)                 \
  compile_context Wasm* __wasm = enter_context(wasm); \
  Context* context = malloc(sizeof(Context));         \
  context->lval = _lval;                              \
  context->msg = _msg;                                \
  __wasm->context->car = context;

#define NEW_CONTEXT_CELL(_msg, _cell)                 \
  compile_context Wasm* __wasm = enter_context(wasm); \
  Context* context = malloc(sizeof(Context));         \
  context->cell = _cell;                              \
  context->msg = _msg;                                \
  __wasm->context->car = context;

void print_context(Wasm* wasm);
#endif  // __WASM_UTIL_H_
