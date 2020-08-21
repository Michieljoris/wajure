#include "misc_fns.h"

#include "assert.h"
#include "eval.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "platform.h"
#include "print.h"
#include "read.h"

int is_lval_type(Lval* lval, int type, int subtype) {
  return lval->type == type && lval->subtype == subtype;
}

Lval* macroexpand(Lenv* env, Lval* lval, int do_recurse) {
  /* Check we have non-empty list where the first expr is a symbol */
  if (is_lval_type(lval, LVAL_COLLECTION, LIST) && list_count(lval->head) > 0 &&
      ((Lval*)(lval->head->car))->type == LVAL_SYMBOL) {
    /* Have a peek at the eval of that symbol */
    scoped Lval* lval_fun = lenv_get(env, lval->head->car);
    /* If it's a macro then eval it with the lval args */
    if (is_lval_type(lval_fun, LVAL_FUNCTION, MACRO)) {
      scoped Lval* arg_list = make_lval_list();
      arg_list->head = retain(lval->head->cdr);
      /* Bind the macro with its args */
      Lval* bound_macro = expand_macro(lval_fun, arg_list);
      /* info("\nbound_macro *******************************************\n"); */
      /* lval_println(bound_macro); */
      /* info("*******************************************\n"); */
      // release fun and args
      if (bound_macro->type == LVAL_ERR) {
        return bound_macro;
      }
      /* Recursively expand  */
      if (do_recurse) {
        Lval* ret = macroexpand(env, bound_macro, do_recurse);
        release(bound_macro);
        return ret;
      } else {
        return bound_macro;
      }
    }
  }

  return retain(lval);
}

Lval* macroexpand_1_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("macroexpand-1", 1)
  ITER_NEXT
  Lval* lval = arg;
  ITER_END
  return macroexpand(env, lval, 0);
}

Lval* macroexpand_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("macroexpand", 1)
  ITER_NEXT
  Lval* lval = arg;
  ITER_END
  return macroexpand(env, lval, 1);
}

Lval* eval_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("eval", 1)
  ITER_NEXT
  Lval* lval = arg;
  ITER_END
  return lval_eval(env, lval);
}

Lval* print_env_fn(Lenv* env, Lval* arg_list) {
  ddebug("print_env:\n");
  env_print(env);
  return make_lval_list();
}

int exit_repl = 0;

Lval* exit_fn(Lenv* e, Lval* arg_list) {
  exit_repl = 1;
  return make_lval_list();
}

#ifndef WASM

char* read_file(char* file_name) {
  // Open file and check it exists
  FILE* f = fopen(file_name, "rb");
  if (f == NIL) return NIL;

  // Read file contents into string
  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* str = calloc(length + 1, 1);
  fread(str, 1, length, f);
  fclose(f);
  return str;
}
#else

char* read_file(char* file_name) { return NULL; }

#endif

Lval* read_string(Lenv* env, char* str) {
  int pos = 0;
  scoped Lval* lval_list = lval_read_list(str, &pos, '\0');
  if (lval_list->type == LVAL_ERR) return retain(lval_list);
  return do_list(env, lval_list, RETURN_ON_ERROR);
}

Lval* read_string_fn(Lenv* env, Lval* arg_list) {
  // TODO: The clojure read-string reads only the first expression, and doesn't
  // eval it
  ITER_NEW_N("read-string", 1)
  ITER_NEXT_TYPE(LVAL_LITERAL, STRING)
  return read_string(env, arg->str);
}

Lval* slurp(Lenv* env, char* file_name) {
#ifndef WASM
  char* str = read_file(file_name);
  if (!str) return make_lval_err("Could not load file %s", str);
  Lval* result = read_string(env, str);
  printf("Slurped: %s \n", file_name);
  free(str);
  return result;
#else
  return NULL;
#endif
}

Lval* slurp_fn(Lenv* env, Lval* arg_list) {
  ddebug("\nload_fn: ");
  ITER_NEW_N("load", 1)
  ITER_NEXT_TYPE(LVAL_LITERAL, STRING)
  return slurp(env, arg->str);
}

LispyFn misc_builtins[] = {{"eval", eval_fn},
                           {"print-env", print_env_fn},
                           {"exit", exit_fn},
                           {"slurp", slurp_fn},
                           /* {"mpc_load", mpc_load_fn}, */
                           {"read-string", read_string_fn},
                           {"macroexpand", macroexpand_fn},
                           {"macroexpand-1", macroexpand_1_fn},
                           {NIL}

};
