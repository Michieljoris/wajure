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

BinaryenExpressionRef wasm_runtime_error(Wasm* wasm, int offset);

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

#define CONTEXT(_msg)                                 \
  compile_context Wasm* __wasm = enter_context(wasm); \
  Context* context = __wasm->context->car;            \
  context->msg = _msg;                                \
  printf("Entering context: %s\n", _msg);

#define CONTEXT_LVAL(_msg, _lval) \
  CONTEXT(_msg)                   \
  context->lval = _lval;

#define CONTEXT_CELL(_msg, _cell) \
  CONTEXT(_msg)                   \
  context->cell = _cell;

#define CONTEXT_FUNCTION(_msg, _fn_name, _local_count)         \
  CONTEXT(_msg)                                                \
  context->function_context = malloc(sizeof(FunctionContext)); \
  context->function_context->fn_name = _fn_name;               \
  context->function_context->closure_count = 0;                \
  context->function_context->closure = lenv_new();             \
  context->function_context->local_count = _local_count;

void print_context(Context* c);
void print_wasm_context(Wasm* wasm);

Lenv* enter_env(Wasm* wasm);

void leave_env(Wasm* wasm);

Lval* make_lval_compiler(Context* context, int subtype, int offset);

#endif  // __WASM_UTIL_H_
