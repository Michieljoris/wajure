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

CFn c_fns[] = {
    // from js
    {NULL, NULL, "log_int", 1, 0},
    {NULL, NULL, "log_string", 1, 0},
    {NULL, NULL, "log_string_n", 2, 0},
    {NULL, NULL, "runtime_error", 2, 0},
    // print
    {NULL, NULL, "lval_print", 1, 0},
    {NULL, NULL, "lval_println", 1, 0},
    // printf
    {NULL, NULL, "printf_", 2, 1},
    // lval
    {NULL, NULL, "make_lval_num", 1, 1},
    {NULL, NULL, "make_lval_nil", 0, 1},
    {NULL, NULL, "make_lval_true", 0, 1},
    {NULL, NULL, "make_lval_false", 0, 1},
    {NULL, NULL, "make_lval_str", 1, 1},
    {NULL, NULL, "make_lval_list", 0, 1},
    {NULL, NULL, "new_lval_list", 1, 1},
    {NULL, NULL, "make_lval_sym", 1, 1},
    {NULL, NULL, "new_lval_vector", 1, 1},
    // lispy_mempool
    {NULL, NULL, "lalloc_size", 1, 1},
    {NULL, NULL, "lalloc_type", 1, 1},
    // refcount
    {NULL, NULL, "retain", 1, 1},
    {NULL, NULL, "release", 1, 0},

    // list
    {NULL, NULL, "prefix_list", 2, 1},

    // lib
    {NULL, NULL, "_strcpy", 2, 1},
    {NULL, NULL, "print_slot_size", 0, 0},
    // runtime
    {NULL, NULL, "wval_print", 1, 0},
    {NULL, NULL, "make_lval_wasm_lambda", 4, 1},
    {NULL, NULL, "get_wval_type", 1, 1},
    {NULL, NULL, "get_wval_subtype", 1, 1},
    {NULL, NULL, "get_wval_fn_table_index", 1, 1},
    {NULL, NULL, "get_wval_closure", 1, 1},
    {NULL, NULL, "get_wval_partials", 1, 1},
    {NULL, NULL, "get_wval_partial_count", 1, 1},
    {NULL, NULL, "listify_args", 2, 1},
    {NULL, NULL, "bundle_rest_args", 3, 0},
    {NULL, NULL, "rewrite_pointers", 3, 0},
    {NULL, NULL, "new_cell", 2, 1},
    {NULL, NULL, "dbg", 1, 0},
    {NULL}};

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
    /* {"rt_error_too_few_args", FTI_RTE_TOO_FEW_ARGS, */
    /*  add_rt_error_too_few_args_fn, NULL}, */
    /* {"rt_error_too_many_args", FTI_RTE_TOO_MANY_ARGS, */
    /*  add_rt_error_too_many_args_fn, NULL}, */
    {"rt_error_not_a_fn", FTI_RTE_NOT_A_FN, add_fn_rt_error_not_a_fn, NULL},
    {"copy_and_retain", FTI_COPY_AND_RETAIN, add_copy_and_retain_fn, NULL},
    {"partial", FTI_PARTIAL, add_partial_fn, compile_partial_call},
    {"apply", FTI_APPLY, add_apply_fn, NULL},
    {"keyword_as_fn", FTI_KEYWORD, add_keyword_fn, NULL},
    {"vector_as_fn", FTI_VECTOR, add_vector_fn, NULL},
    {"set_as_fn", FTI_SET, add_set_fn, NULL},
    {"symbol_as_fn", FTI_SYMBOL, add_symbol_fn, NULL},
    {"map_as_fn", FTI_MAP, add_map_fn, NULL},
    {NULL}};

void _register_c_fns(CFn c_fns[]) {
  int i = 0;
  char* c_fn_name;
  do {
    c_fn_name = c_fns[i].c_fn_name;
    if (!c_fn_name) break;  // end of list
    if (c_fns[i].wajure_fn_name)
      state->wajure_to_c_fn_map = alist_prepend(
          state->wajure_to_c_fn_map, c_fns[i].wajure_fn_name, &c_fns[i]);
    i++;
  } while (1);
}

void add_c_fn_imports(Wasm* wasm, CFn c_fns[]) {
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
    /* if (c_fns[i].wajure_fn_name) */
    /*   state->wajure_to_c_fn_map = alist_prepend( */
    /*       state->wajure_to_c_fn_map, c_fns[i].wajure_fn_name, &c_fns[i]); */
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
  Ber body = call_c_fn;
  int locals_count = 0;
  BinaryenAddFunction(module, c_fn.wajure_fn_name, params_type, results_type,
                      NULL, locals_count, body);

  c_fn.fn_table_index = add_fn_to_table(wasm, c_fn.wajure_fn_name);

  char* data_lval = make_data_lval(wasm, NULL, c_fn.fn_table_index);
  c_fn.data_offset =
      inter_data_lval(wasm, data_lval) + wasm->builtins_data_start;

  write_symbol_table_line(wasm, LVAL_FUNCTION, c_fn.wajure_fn_name,
                          c_fn.data_offset, c_fn.fn_table_index, 1, 1);
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
  int fns_count = 0;
  while (wasm_fns[fns_count].wasm_fn_name) fns_count++;
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
    write_symbol_table_line(wasm, LVAL_FUNCTION, fn_name, wasm_fn.data_offset,
                            wasm_fn.fn_table_index, -1, -1);
    /* printf("wasm_fn name: %s index: %d data_offset: %d\n",
     * wasm_fn.wasm_fn_name, */
    /*        wasm_fn.fn_table_index, wasm_fn.data_offset); */
  }
}

void assign_offsets_to_builtins() {
  // We read the symbol table custom section of builtin
  FILE* fp;
  char command[1000];

  char* wasm_file_name = builtin_to_wasm(config->out_wasm, config->builtin);
  sprintf(command, "node custom.js %s symbol_table\n", wasm_file_name);
  release(wasm_file_name);
  fp = popen(command, "r");

  char fn_name[1000];
  int data_offset, fn_table_index, param_count, has_rest_arg;
  // Line by line
  do {
    char line[1000];
    int ret = fscanf(fp, "%s", line);
    if (ret > 0) {
      strsubst(line, ',', ' ');
      char type[100];
      sscanf(line, "%s %s %d %d %d %d", fn_name, type, &data_offset,
             &fn_table_index, &param_count, &has_rest_arg);

      CFn* c_fn =
          (CFn*)alist_get(state->wajure_to_c_fn_map, is_eq_str, fn_name);
      if (c_fn) {
        /* printf("assigning %d %d to %s\n", data_offset, fn_table_index,
         * fn_name); */
        c_fn->fn_table_index = fn_table_index;
        c_fn->data_offset = data_offset;
      } else {
        // See if it's one of our native fns
        WasmFn* native_fn = (WasmFn*)alist_get(state->wajure_to_native_fn_map,
                                               is_eq_str, fn_name);

        // If so set its fn_table_index
        if (native_fn) {
          /* native_fn->fn_table_index = fn_table_index; */
          native_fn->data_offset = data_offset;
        }
      }
    } else
      break;
  } while (1);
  fclose(fp);
  /* int fns_count = 0; */
  /* while (wasm_fns[fns_count].wasm_fn_name) fns_count++; */
  /* for (int i = 0; i < fns_count; i++) */
  /*   printf("%s %d %d\n", wasm_fns[i].wasm_fn_name, wasm_fns[i].data_offset,
   */
  /*          wasm_fns[i].fn_table_index); */
  /* fns_count = 0; */
  /* while (list_c_fns[fns_count].c_fn_name) fns_count++; */
  /* for (int i = 0; i < fns_count; i++) */
  /*   printf("%s %d %d\n", list_c_fns[i].c_fn_name, list_c_fns[i].data_offset,
   */
  /*          list_c_fns[i].fn_table_index); */
}

void register_native_fns() {
  int fns_count = 0;
  while (wasm_fns[fns_count].wasm_fn_name) fns_count++;
  for (int i = 0; i < fns_count; i++) {
    /* printf("%s %d %d\n", wasm_fns[i].wasm_fn_name,
     * wasm_fns[i].fn_table_index, */
    /*        wasm_fns[i].data_offset); */
    state->wajure_to_native_fn_map = alist_prepend(
        state->wajure_to_native_fn_map, wasm_fns[i].wasm_fn_name, &wasm_fns[i]);
  };
}

char* builtin_to_wat(char* path, char* name) {
  char* file_name = lalloc_size(_strlen(path) + _strlen("/builtin.wat") + 1);
  sprintf(file_name, "%s/%s.wat", path, name);
  return file_name;
}

char* builtin_to_wasm(char* path, char* name) {
  char* file_name = lalloc_size(_strlen(path) + _strlen("/builtin.wasm") + 1);
  sprintf(file_name, "%s/%s.wasm", path, name);
  return file_name;
}

void import_c_fns(Wasm* wasm) {
  add_c_fn_imports(wasm, c_fns);
  add_c_fn_imports(wasm, math_c_fns);
  add_c_fn_imports(wasm, list_c_fns);
  add_c_fn_imports(wasm, util_c_fns);
}

void register_c_fns() {
  _register_c_fns(c_fns);
  _register_c_fns(math_c_fns);
  _register_c_fns(list_c_fns);
  _register_c_fns(util_c_fns);
}

void make_builtin_module(char* path, char* name) {
  Wasm* wasm = init_wasm();
  register_native_fns();
  add_wasm_fns(wasm);
  import_c_fns(wasm);

  add_c_fn_wrappers(wasm, list_c_fns);
  add_c_fn_wrappers(wasm, math_c_fns);
  add_c_fn_wrappers(wasm, util_c_fns);

  add_function_table(wasm);
  add_memory_section(wasm);

  BinaryenAddCustomSection(wasm->module, "symbol_table", wasm->symbol_table,
                           wasm->symbol_table_count);
  add_deps_custom_section(wasm, "deps", wasm->deps);

  char* size_str = malloc(100);
  sprintf(size_str, "%d", wasm->data_offset);
  BinaryenAddCustomSection(wasm->module, "data_size", size_str,
                           _strlen(size_str));
  sprintf(size_str, "%d", wasm->fns_count);
  BinaryenAddCustomSection(wasm->module, "fn_table_size", size_str,
                           _strlen(size_str));
  free(size_str);
  // No rewriting necessary:
  // fn_table_offset is 0 by design
  // partials and closure pointers are not used for native fns.
  // we're not interring any lvals in the builtin module
  wasm->lval_offsets_count = 0;
  wasm->cell_offsets_count = 0;
  // But we do need to go through the motions
  inter_rewrite_info(wasm);

  printf("writing wat and wasm!!!! %s/%s\n", path, name);
  char* wat_file_name = builtin_to_wat(path, name);
  write_wat(wasm, wat_file_name);
  char* wasm_file_name = builtin_to_wasm(path, name);
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
