#include "compile.h"

#include <binaryen-c.h>

#include "env.h"
#include "fns.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "misc_fns.h"
#include "print.h"
/* #include "misc_fns.h" */
/* #include "print.h" */
#include "read.h"
/* #include "refcount.h" */
#include "wasm.h"
#include "wasm_util.h"

char* make_err(char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  char* str = lalloc_size(512);
  vsnprintf(str, 511, fmt, va);
  str = lrealloc(str, _strlen(str) + 1);
  va_end(va);
  return str;
}

int compile_expression(BinaryenModuleRef module, Lval* lval_expr) {
  printf("LVAL:");
  lval_println(lval_expr);
  /* info("\n->>>>>>>>EVAL: "); */
  /* lval_infoln(lval); */
  /* printf("lval->type: %s\n", lval_type_constant_to_name(lval->type)); */
  /* Lval* ret = NIL; */
  /* switch (lval->type) { */
  /*   case LVAL_SYMBOL: */
  /*     return eval_symbol(env, lval); */
  /*   case LVAL_COLLECTION: */
  /*     switch (lval->subtype) { */
  /*       case LIST: */
  /*         return compile_fn_call(env, lval); */
  /*       case VECTOR: */
  /*         return compile_vector(env, lval); */
  /*       case MAP: */
  /*         /\* TODO: *\/ */
  /*         return lval; */
  /*       default: */
  /*         return make_err("Unknown seq subtype: %d ", lval->subtype); */
  /*     } */
  /*     break; */
  /*   default: */
  /*     return retain(lval); */
  /* } */
  return 1;
}

int strings_data_cat(Wasm* wasm, char* str) {
  int len = _strlen(str) + 1;
  /* printf("%d %d\n", wasm->strings_offset, len); */
  wasm->strings = realloc(wasm->strings, wasm->strings_offset + len);
  _strncpy(wasm->strings + wasm->strings_offset, str, len);
  wasm->strings_offset += len;
  /* printf("strings_offset: %d\n", wasm->strings_offset); */
  return wasm->strings_offset;
}

/* void add_global_section(BinaryenModuleRef module) { */
/*   BinaryenAddGlobalImport(module, "data_end", "env", "data_end", */
/*                           BinaryenTypeInt32(), 0); */

/*   BinaryenAddGlobal(module, "a-global", BinaryenTypeInt32(), 0, */
/*                     make_int32(module, 7)); */

/*   BinaryenAddGlobal(module, "a-mutable-global", BinaryenTypeFloat32(), 1, */
/*                     make_int32(module, 123)); */
/* } */

int compile_list(Wasm* wasm, Lval* list) {
  BinaryenModuleRef module = wasm->module;
  scoped_iter Cell* i = iter_new(list);
  Lval* lval = iter_next(i);
  int result;
  while (lval) {
    result = compile_expression(module, lval);
    if (!result) {
      lval_print(lval);
      return result;
    }
    lval = iter_next(i);
  }
  return 1;
}

Lval* parse_file(char* file_name) {
  char* str = read_file(file_name);
  if (!str) {
    printf("Could not load file %s", str);
    return NULL;
  }

  int pos = 0;
  Lval* lval_list = lval_read_list(str, &pos, '\0');
  if (lval_list->type == LVAL_ERR) {
    lval_println(lval_list);
    return NULL;
  }
  return lval_list;
}

int compile(char* file_name) {
  info("Compiling %s\n", file_name);

  set_log_level(LOG_LEVEL_INFO);

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

  /* info("Slurping %s\n", file_name); */
  Lval* result = slurp(user_env, file_name);

  printf("Result of slurping %s:\n", file_name);
  lval_println(result);
  env_print(user_env);
  /* env_print(root_env); */

  release(result);

  release(user_env->kv);
  user_env->kv = NIL;
  release(user_env);

  release(stdlib_env->kv);
  stdlib_env->kv = NIL;
  release(stdlib_env);

  release(root_env);

  return 1;
  Lval* lval_list = parse_file(file_name);

  // Wasm
  Wasm* wasm = init_wasm();
  import_runtime(wasm);

  compile_list(wasm, lval_list);
  add_test_fn(wasm);

  strings_data_cat(wasm, "foo3");
  strings_data_cat(wasm, "bar");
  /* printf("strings_data: %s\n", wasm->strings + 4); */

  BinaryenAddFunctionExport(wasm->module, "test", "test");

  // We only have this data _after_ compiling
  /* add_global_section(module); */
  /* add_function_table(module); */
  add_memory_section(wasm);

  /* BinaryenModulePrint(wasm->module); */

  write_wat(wasm, "compiled/lispy.wat");
  write_wasm(wasm, "compiled/lispy.wasm");

  free_wasm(wasm);
  return 1;
}
