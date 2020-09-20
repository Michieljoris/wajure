#ifndef __WASM_UTIL_H_
#define __WASM_UTIL_H_

#include <binaryen-c.h>

#include "ltypes.h"
#include "platform.h"
#include "wasm.h"

CResult wasm_retain(Wasm* wasm, Ber wval);
CResult wasm_retain_and_drop(Wasm* wasm, Ber wval);
CResult wasm_release(Wasm* wasm, Ber wval);

Ber make_int32(BinaryenModuleRef module, int x);
void write_wat(Wasm* wasm, char* file_name);

void write_wasm(Wasm* wasm, char* file_name);
void add_test_fn(Wasm* wasm);

int add_string_to_data(Wasm* wasm, char* str);

int add_bytes_to_data(Wasm* wasm, char* data, int len);
int add_fn_to_table(Wasm* wasm, char* fn_name);

Ber wasm_offset(Wasm* wasm, int offset);

Ber wasm_log_int(Wasm* wasm, Ber int32);

Ber wasm_printf(Wasm* wasm, int offset);

BinaryenType* make_type_int32_array(int count);

BinaryenType make_type_int32(int count);

Ber wasm_offset(Wasm* wasm, int offset);

Wasm* enter_context(Wasm* wasm);

void leave_context(Wasm* wasm);

#define compile_context __attribute__((__cleanup__(_leave_context)))
void _leave_context(void* data);

#define CONTEXT(_msg)                                 \
  compile_context Wasm* __wasm = enter_context(wasm); \
  Context* context = __wasm->context->car;            \
  context->msg = _msg;                                \
  context->cell = NULL;                               \
  /* printf("Entering context: %s\n", _msg); */

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

Lval* make_lval_ref(Context* context, int subtype, int offset);

CResult wasm_runtime_error(Wasm* wasm, int err_no, char* msg);

char* uniquify_name(Wasm* wasm, char* name);

typedef struct {
  int* local_indices;
  int local_indices_count;
} LocalIndices;

LocalIndices* li_init();

int li_new(Wasm* wasm);
int li_get(Wasm* wasm);
int li_track(Wasm* wasm, LocalIndices* li, int index);
CResult li_release(Wasm* wasm, LocalIndices* li, char* name);

CResult li_result_with_release(Wasm* wasm, LocalIndices* li, char* name,
                               Ber release_locals, Ber result);
void li_close(LocalIndices* li);

CResult cresult(Ber ber);
CResult cnull();

CResult wasm_lalloc_type(Wasm* wasm, int type);

CResult wasm_lalloc_size(Wasm* wasm, int size);

char* make_global_name(char* prefix, char* namespace, char* name);

void add_dep(Wasm* wasm, char* global_name);

char* number_fn_name(Wasm* wasm, char* fn_name);

#endif  // __WASM_UTIL_H_
