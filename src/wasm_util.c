#include "wasm_util.h"

#include <binaryen-c.h>

#include "env.h"
#include "fns.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "ltypes.h"
#include "misc_fns.h"
#include "platform.h"
#include "wasm.h"

void write_string(char* file_name, char* str) {
  FILE* f = fopen(file_name, "wb");
  fputs(str, f);
  fclose(f);
}
void write_binary(char* file_name, void* bin, size_t size) {
  FILE* f = fopen(file_name, "wb");
  fwrite(bin, 1, size, f);
  fclose(f);
}

void write_wat(Wasm* wasm, char* file_name) {
  BinaryenSetColorsEnabled(0);
  char* output = BinaryenModuleAllocateAndWriteText(wasm->module);
  /* printf("%s %d\n", output, _strlen(output)); */
  write_string(file_name, output);
  free(output);
  BinaryenSetColorsEnabled(1);
}

void write_wasm(Wasm* wasm, char* file_name) {
  BinaryenModuleRef module = wasm->module;
  BinaryenModuleAllocateAndWriteResult result =
      BinaryenModuleAllocateAndWrite(module, NULL);

  void* binary = result.binary;
  size_t binaryBytes = result.binaryBytes;
  char* sourceMap = result.sourceMap;
  /* printf("%s \n%zu \n%s\n", binary, binaryBytes, sourceMap); */
  write_binary(file_name, binary, binaryBytes);
  free(binary);
  free(sourceMap);
}

BinaryenExpressionRef make_int32(BinaryenModuleRef module, int x) {
  return BinaryenConst(module, BinaryenLiteralInt32(x));
}

BinaryenExpressionRef wasm_log_int(Wasm* wasm, int int32) {
  BinaryenModuleRef module = wasm->module;

  BinaryenExpressionRef operands[] = {make_int32(module, int32)};

  BinaryenExpressionRef log_int =
      BinaryenCall(module, "log_int", operands, 1, BinaryenTypeNone());

  return log_int;
}

/* BinaryenExpressionRef wasm_offset_old(Wasm* wasm, int offset) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   BinaryenExpressionRef wasm_data_end = */
/*       BinaryenGlobalGet(module, "__data_end", BinaryenTypeInt32()); */

/*   BinaryenExpressionRef wasm_offset = make_int32(module, offset); */

/*   BinaryenExpressionRef str_p = */
/*       BinaryenBinary(module, BinaryenAddInt32(), wasm_data_end, wasm_offset);
 */
/*   return str_p; */
/* } */

BinaryenExpressionRef wasm_offset(Wasm* wasm, int offset) {
  return make_int32(wasm->module, wasm->__data_end + offset);
}

BinaryenExpressionRef wasm_log_string(Wasm* wasm, int offset) {
  BinaryenModuleRef module = wasm->module;

  BinaryenExpressionRef operands[] = {wasm_offset(wasm, offset)};

  BinaryenExpressionRef log_string =
      BinaryenCall(module, "log_string", operands, 1, BinaryenTypeNone());

  return log_string;
}

BinaryenExpressionRef wasm_log_string_n(Wasm* wasm, int offset, int n) {
  BinaryenModuleRef module = wasm->module;

  BinaryenExpressionRef operands[] = {wasm_offset(wasm, offset),
                                      make_int32(module, n)};

  BinaryenExpressionRef log_string_n =
      BinaryenCall(module, "log_string_n", operands, 2, BinaryenTypeNone());

  return log_string_n;
}

BinaryenExpressionRef wasm_printf(Wasm* wasm, int offset) {
  BinaryenModuleRef module = wasm->module;

  BinaryenExpressionRef operands[] = {wasm_offset(wasm, offset),
                                      make_int32(module, 0)};

  BinaryenExpressionRef printf =
      BinaryenCall(module, "printf", operands, 2, BinaryenTypeNone());

  BinaryenExpressionRef drop = BinaryenDrop(module, printf);
  return drop;
}

int add_bytes_to_data(Wasm* wasm, char* str, int len) {
  int offset = wasm->strings_offset;
  /* printf("%d %d\n", wasm->strings_offset, len); */
  wasm->strings = realloc(wasm->strings, offset + len);
  _memmove(wasm->strings + offset, str, len);

  wasm->strings_offset += len;
  /* printf("strings_offset: %d\n", wasm->strings_offset); */
  return offset;
}

int add_string_to_data(Wasm* wasm, char* str) {
  int len = _strlen(str) + 1;
  int offset = wasm->strings_offset;
  /* printf("%d %d\n", wasm->strings_offset, len); */
  wasm->strings = realloc(wasm->strings, offset + len);
  _strncpy(wasm->strings + offset, str, len);

  wasm->strings_offset += len;
  /* printf("strings_offset: %d\n", wasm->strings_offset); */
  return offset;
}

/* int add_lval_to_data(Wasm* wasm, Lval* lval) { */
/*   /\* int len = _strlen(str) + 1; *\/ */
/*   /\* /\\* printf("%d %d\n", wasm->strings_offset, len); *\\/ *\/ */
/*   /\* wasm->strings = realloc(wasm->strings, wasm->strings_offset + len); *\/
 */
/*   /\* _strncpy(wasm->strings + wasm->strings_offset, str, len); *\/ */
/*   /\* wasm->strings_offset += len; *\/ */
/*   /\* /\\* printf("strings_offset: %d\n", wasm->strings_offset); *\\/ *\/ */
/*   return wasm->lvals_offset; */
/* } */

int add_fn_to_table(Wasm* wasm, char* fn_name) {
  int free_fn_slot = wasm->fns_count;
  wasm->fns_count++;
  wasm->fn_names = realloc(wasm->fn_names, wasm->fns_count * sizeof(char*));
  int len = _strlen(fn_name) + 1;
  wasm->fn_names[free_fn_slot] = malloc(len);
  _strncpy(wasm->fn_names[free_fn_slot], fn_name, len);
  return free_fn_slot;
}

Lenv* interprete_file(char* file_name) {
  Lenv* root_env = lenv_new();
  lenv_add_builtin_fns(root_env);

  Lenv* stdlib_env = lenv_new();
  stdlib_env->parent_env = retain(root_env);

  /* info("Slurping lispy/stdlib.lispy \n"); */
  stdlib_env->is_user_env = 1;
  slurp(stdlib_env, "lispy/stdlib.lispy");

  Lenv* user_env = lenv_new();
  user_env->parent_env = retain(stdlib_env);
  stdlib_env->is_user_env = 0;
  user_env->is_user_env = 1;

  release(slurp(user_env, file_name));
  // stdlib is only used to interprete
  user_env->parent_env = root_env;
  return user_env;
}

void release_env(Lenv* env) {
  release(env->kv);
  env->kv = NIL;
  Lenv* parent_env = env->parent_env;
  release(env);
  if (parent_env) release_env(parent_env);
}

void add_test_fn(Wasm* wasm) {
  BinaryenModuleRef module = wasm->module;

  add_string_to_data(wasm, "foo3");
  add_string_to_data(wasm, "bar");
  /* printf("strings_data: %s\n", wasm->strings); */
  /* printf("strings_data: %s\n", wasm->strings + _strlen("foo3") + 1); */

  BinaryenType localTypes[] = {};
  /* BinaryenType localTypes[] = {BinaryenTypeInt32(), BinaryenTypeInt32()}; */

  /* int offset = 5; */
  /* BinaryenExpressionRef printf = wasm_printf(wasm, offset); */
  /* BinaryenExpressionRef log_int = wasm_log_int(wasm, 123); */
  /* BinaryenExpressionRef log_string = wasm_log_string(wasm, offset); */
  /* BinaryenExpressionRef log_string_n = wasm_log_string_n(wasm, offset, 2); */

  /* BinaryenExpressionRef my_value_list[] = {printf, log_int, log_string, */
  /*                                          log_string_n}; */
  BinaryenType TypeNone = BinaryenTypeNone();
  BinaryenType TypeInt32x1 = make_type_int32(1);
  BinaryenExpressionRef my_value_list[] = {};
  BinaryenExpressionRef body =
      BinaryenBlock(module, "my-block", my_value_list,
                    sizeof(my_value_list) / sizeof(BinaryenExpressionRef),
                    BinaryenTypeAuto());

  BinaryenAddFunction(module, "test", TypeNone, TypeInt32x1, localTypes,
                      sizeof(localTypes) / sizeof(BinaryenType), body);

  BinaryenAddFunctionExport(wasm->module, "test", "test");
}

BinaryenType* make_type_int32_array(int count) {
  BinaryenType* types = malloc(count * sizeof(BinaryenTypeInt32()));
  while (count--) types[count] = BinaryenTypeInt32();
  return types;
}

BinaryenType make_type_int32(int count) {
  BinaryenType* _type = make_type_int32_array(count);
  return BinaryenTypeCreate(_type, count);
}

int slot_type_size = 4 * 4;
int lval_type_size = 5 * 4;  // type and subtype are in 4 bytes
int ref_count_offset = 0;
int data_p_offset = 3;
int type_offset = 4;
int num_offset = 5;
int str_offset = 6;
int head_offset = 7;
int hash_offset = 8;

BinaryenExpressionRef make_lval_literal(Wasm* wasm, Lval* lval) {
  int data_lval_size = slot_type_size + lval_type_size;
  int* data_lval = calloc(1, data_lval_size);
  int string_offset = 0;
  if (lval->str) string_offset = add_string_to_data(wasm, lval->str);

  data_lval[ref_count_offset] = 1;
  data_lval[data_p_offset] = wasm->__data_end + wasm->strings_offset + 16;
  data_lval[type_offset] = lval->type | lval->subtype << 8;
  data_lval[num_offset] = lval->num;
  data_lval[str_offset] = wasm->__data_end + string_offset;
  data_lval[head_offset] = 0;  //(int)lval->head;  // TODO: add list to data!!!
  data_lval[hash_offset] = lval->hash;

  int offset = add_bytes_to_data(wasm, (char*)data_lval, data_lval_size);

  return make_int32(wasm->module, wasm->__data_end + offset + slot_type_size);
}
