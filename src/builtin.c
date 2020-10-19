#include "builtin.h"

#include "compile.h"
#include "compile_partial.h"
#include "inter.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"
#include "namespace.h"
#include "native.h"
#include "print.h"
#include "state.h"
#include "wasm.h"
#include "wasm_util.h"

/* CFn */
/* char* wajure_fn_name; */
/* Lbuiltin fun; */
/* char* c_fn_name; */
/* int params_count; */
/* int results_count; */
/* int fn_table_index; */
/* int data_offset; */
extern CFn c_fns[];
extern CFn list_c_fns[];
extern CFn math_c_fns[];
extern CFn util_c_fns[];

/* WasmFn: { */
/*   char* wasm_fn_name; */
/*   int fn_table_index; */
/*   void (*add_fn)(Wasm*, char*); */
/*   CResult (*compile_fn_call)(Wasm*, NativeFn, Cell*); */
/* int data_offset; */
/* }; */
WasmFn wasm_fns[] = {
    {"rt_error_too_few_args", FTI_RTE_TOO_FEW_ARGS,
     add_rt_error_too_few_args_fn, NULL},
    {"rt_error_too_many_args", FTI_RTE_TOO_MANY_ARGS,
     add_rt_error_too_many_args_fn, NULL},
    {"rt_error_not_a_fn", FTI_RTE_NOT_A_FN, add_fn_rt_error_not_a_fn, NULL},
    {"copy_and_retain", FTI_COPY_AND_RETAIN, add_copy_and_retain_fn, NULL},
    {"partial", FTI_PARTIAL, add_partial_fn, compile_partial_call},
    {"apply", FTI_APPLY, add_apply_fn, NULL},
    {"keyword_as_fn", FTI_KEYWORD, add_keyword_fn, NULL},
    {"vector_as_fn", FTI_VECTOR, add_vector_fn, NULL},
    {"set_as_fn", FTI_SET, add_set_fn, NULL},
    {"symbol_as_fn", FTI_SYMBOL, add_symbol_fn, NULL},
    {"map_as_fn", FTI_MAP, add_map_fn, NULL},
};

void add_c_fn_imports2(Wasm* wasm, CFn c_fns[]) {
  BinaryenModuleRef module = wasm->module;
  int i = 0;
  char* c_fn_name;
  int params_count, results_count;
  do {
    c_fn_name = c_fns[i].c_fn_name;
    if (!c_fn_name) break;  // end of list
    params_count = c_fns[i].params_count;
    results_count = c_fns[i].results_count;
    BinaryenAddFunctionImport(module, c_fn_name, "env", c_fn_name,
                              make_type_int32(params_count),
                              make_type_int32(results_count));
    if (c_fns[i].wajure_fn_name)
      state->wajure_to_c_fn_map = alist_prepend(
          state->wajure_to_c_fn_map, c_fns[i].wajure_fn_name, c_fn_name);
    i++;
  } while (1);
}

void add_c_fn_wrapper(Wasm* wasm, CFn c_fn) {
  BinaryenModuleRef module = wasm->module;

  /* int closure_param = 0; */
  int args_block_ptr_param = 1;
  int args_count_param = 2;

  // Call c fn with args bundled into a list.
  Ber listify_args_operands[] = {local_get_int32(module, args_block_ptr_param),
                                 local_get_int32(module, args_count_param)};
  Ber args_as_list = BinaryenCall(module, "listify_args", listify_args_operands,
                                  2, BinaryenTypeInt32());

  Ber c_fn_operands[2] = {make_int32(wasm->module, 0), args_as_list};
  Ber call_c_fn = BinaryenCall(module, c_fn.c_fn_name, c_fn_operands, 2,
                               make_type_int32(1));

  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(1);
  /* char* wasm_fn_name = malloc(_strlen(c_fn.wajure_fn_name) + 3); */
  /* sprintf(wasm_fn_name, "%s", c_fn.wajure_fn_name); */
  Ber body = call_c_fn;
  int locals_count = 0;
  BinaryenAddFunction(module, c_fn.wajure_fn_name, params_type, results_type,
                      NULL, locals_count, body);

  c_fn.fn_table_index = add_fn_to_table(wasm, c_fn.wajure_fn_name);

  /* free(wasm_fn_name); */

  char* data_lval = make_data_lval(wasm, NULL, c_fn.fn_table_index);
  c_fn.data_offset =
      inter_data_lval(wasm, data_lval) + wasm->builtins_data_start;
}

void add_c_fn_wrappers(Wasm* wasm, CFn c_fns[]) {
  int i = 0;
  do {
    char* c_fn_name = c_fns[i].c_fn_name;
    if (!c_fn_name) break;  // end of list
    add_c_fn_wrapper(wasm, c_fns[i]);
    i++;
  } while (1);
}

void add_native_fn_to_table(Wasm* wasm, char* fn_name, int index) {
  int len = _strlen(fn_name) + 1;
  wasm->fn_names[index] = malloc(len);
  _strncpy(wasm->fn_names[index], fn_name, len);
}

void add_wasm_fns(Wasm* wasm) {
  int fns_count = sizeof(wasm_fns) / sizeof(*wasm_fns);
  wasm->fns_count += fns_count;
  wasm->fn_names = calloc(fns_count, sizeof(char*));

  for (int i = 0; i < fns_count; i++) {
    WasmFn wasm_fn = wasm_fns[i];
    wasm_fn.add_fn(wasm, wasm_fn.wasm_fn_name);
    add_native_fn_to_table(wasm, wasm_fn.wasm_fn_name, wasm_fn.fn_table_index);
    char* data_lval = make_data_lval(wasm, NULL, wasm_fn.fn_table_index);
    wasm_fn.data_offset =
        inter_data_lval(wasm, data_lval) + wasm->builtins_data_start;
    char* fn_name = wasm_fn.wasm_fn_name;
    char* type_str = "Function";
    int max_len = 1024;
    char* line = malloc(max_len);
    snprintf(line, max_len, "%s,%s,%d,%d,%d,%d\n", fn_name, type_str,
             wasm_fn.data_offset, wasm_fn.fn_table_index, -1, -1);

    int line_count = _strlen(line);
    wasm->symbol_table =
        realloc(wasm->symbol_table, wasm->symbol_table_count + line_count);
    _strncpy(wasm->symbol_table + wasm->symbol_table_count, line, line_count);
    free(line);
    wasm->symbol_table_count += line_count;
    /* printf("wasm_fn name: %s index: %d data_offset: %d\n",
     * wasm_fn.wasm_fn_name, */
    /*        wasm_fn.fn_table_index, wasm_fn.data_offset); */
  }
}

char* builtin_to_wat(char* path) {
  char* file_name = lalloc_size(_strlen(path) + _strlen("/builtin.wat") + 1);
  sprintf(file_name, "%s/builtin.wat", path);
  return file_name;
}

char* builtin_to_wasm(char* path) {
  char* file_name = lalloc_size(_strlen(path) + _strlen("/builtin.wasm") + 1);
  sprintf(file_name, "%s/builtin.wasm", path);
  return file_name;
}

void make_builtin_module(char* path) {
  Wasm* wasm = init_wasm();
  add_wasm_fns(wasm);
  add_c_fn_imports2(wasm, c_fns);
  add_c_fn_imports2(wasm, list_c_fns);
  add_c_fn_imports2(wasm, math_c_fns);
  add_c_fn_imports2(wasm, util_c_fns);

  add_c_fn_wrappers(wasm, list_c_fns);
  add_c_fn_wrappers(wasm, math_c_fns);
  add_c_fn_wrappers(wasm, util_c_fns);

  add_function_table(wasm);
  add_memory_section(wasm);

  char* size_str = malloc(100);
  sprintf(size_str, "%d", wasm->data_offset);
  BinaryenAddCustomSection(wasm->module, "data_size", size_str,
                           _strlen(size_str));
  sprintf(size_str, "%d", wasm->fns_count);
  BinaryenAddCustomSection(wasm->module, "fn_table_size", size_str,
                           _strlen(size_str));
  free(size_str);

  printf("writing wat and wasm!!!! %s\n", path);
  char* wat_file_name = builtin_to_wat(path);
  write_wat(wasm, wat_file_name);
  char* wasm_file_name = builtin_to_wasm(path);
  write_wasm(wasm, wasm_file_name);

  printf("Validating %s\n", wasm_file_name);
  int validate_result = BinaryenModuleValidate(wasm->module);
  if (!validate_result) {
    printf("VALIDATE ERROR!!!!!!\n");
    exit(1);
  } else
    printf("VALIDATED OK!!\n");
  /* BinaryenModulePrint(wasm->module); */

  release(wat_file_name);
  release(wasm_file_name);
  free_wasm(wasm);
}
