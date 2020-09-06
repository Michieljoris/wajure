#include "misc_fns.h"

#include "assert.h"
#include "compile.h"
#include "eval.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "platform.h"
#include "print.h"
#include "read.h"
#include "state.h"

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

/* Lval* read_string(Lenv* env, char* str) { */
/*   int pos = 0; */
/*   scoped Lval* lval_list = lval_read_list(str, &pos, '\0'); */
/*   if (lval_list->type == LVAL_ERR) return retain(lval_list); */
/*   return do_list(env, lval_list, RETURN_ON_ERROR); */
/* } */

Lval* read_string_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("read-string", 1)
  ITER_NEXT_TYPE(LVAL_LITERAL, STRING)
  int pos = 0;
  return read_expr(arg->str, &pos, '\0');
  /* return read_string(env, arg->str, &pos); */
}

Lval* load(Lenv* env, char* file_name) {
#ifndef WASM
  char* str = read_file(file_name);
  if (!str) return make_lval_err("Could not load file %s", file_name);
  int pos = 0;
  Lval* lval = NULL;
  Lval* result = NULL;
  printf("loading %s\n", file_name);
  do {
    lval = read_expr(str, &pos, '\0');
    if (!lval) break;
    release(result);
    if (lval->type == LVAL_ERR) {
      lval_println(lval);
      result = retain(lval);
    } else {
      result = lval_eval(env, lval);
      if (result->type == LVAL_ERR) {
        printf("Encountered error while evalling expression: \n");
        lval_println(lval);
        lval_println(result);
      }
    }
    release(lval);
  } while (1);

  printf("Loaded: %s \n", file_name);
  free(str);
  return result;
#else
  return NULL;
#endif
}

Lval* load_fn(Lenv* env, Lval* arg_list) {
  ddebug("\nload_fn: ");
  ITER_NEW_N("load", 1)
  ITER_NEXT_TYPE(LVAL_LITERAL, STRING)
  return load(env, arg->str);
}

Lval* compile_fn(Lenv* env, Lval* arg_list) {
  ddebug("\ncompile_fn: ");
  ITER_NEW_N("compile", 1)
  ITER_NEXT_TYPE(LVAL_LITERAL, STRING)
  compile(arg->str);
  return make_lval_list();
}

Lval* in_ns_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("in-ns", 1);
  ITER_NEXT_TYPE(LVAL_SYMBOL, -1)
  Lval* lval_symbol = arg;
  ITER_END
  /* Lenv* some_env = alist_get(state->namespaces, is_eq_str, lval_symbol->str);
   */

  scoped char* str = lalloc_size(5);
  _strcpy(str, "*ns*");
  Lval* lval_ns = make_lval_sym(str);
  lenv_put(env, lval_ns, make_lval_namespace(lval_symbol->str));
  return make_lval_nil();
}

Lenv* require_file(Lenv* env, char* file_name) {
  /* Lenv* refer_env = lenv_new(); */
  /* refer_env->parent_env = retain(env); */

  Lenv* ns_env = lenv_new();
  ns_env->parent_env = retain(env);

  ns_env->is_ns_env = 1;
  scoped Lval* result = load(ns_env, file_name);
  if (result->type == LVAL_ERR) {
    lval_println(result);
    exit(1);
  }
  /* release(refer_env->kv); */
  /* refer_env->kv = NULL; */
  /* release(refer_env); */

  /* ns_env->parent_env = NULL; */

  /* release(env); */
  /* printf("Required %s\n", file_name); */
  return ns_env;
}

// A more limited and restricted version of clojure's require.
Lval* require_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("require", 1);
  ITER_NEXT_TYPE(LVAL_COLLECTION, VECTOR)
  Lval* vector = arg;
  ITER_END
  Cell* head = vector->head;
  Lval* required_namespace_sym = NULL;
  char* as = NULL;
  Lval* refer = NULL;
  Namespace* current_namespace = get_namespace_for_env(env);
  if (!current_namespace)
    return make_lval_err(
        "Expecting *ns* to be defined (call in-ns) before require is "
        "called");

  if (list_count(env->kv) > 1)
    return make_lval_err(
        "Require calls can only to be made before any def calls");
  if (list_count(vector->head) > 5)
    return make_lval_err("Too many args passed to require (>5)");

  if (head && ((Lval*)head->car)->type == LVAL_SYMBOL) {
    required_namespace_sym = ((Lval*)head->car);
  } else {
    return make_lval_err(
        "Expecting namespace symbol as first element of quoted vector passed "
        "to require.");
  }

  // Loop through require vector and assign values to 'as' and 'refer' if found.
  head = head->cdr;
  while (head) {
    if (((Lval*)head->car)->subtype != KEYWORD)
      return make_lval_err(
          "Expecting a keyword such as :as or :refer in quoted vector passed "
          "to require");
    Lval* kw = head->car;
    head = head->cdr;
    if (!head)
      return make_lval_err("Expecting arg after keyword :%s in require",
                           kw->str);
    if (_strcmp(kw->str, "as") == 0) {
      if (as)
        return make_lval_err(
            "Expecting only one :as keyword in require vector");
      as = ((Lval*)head->car)->str;
    } else if (_strcmp(kw->str, "refer") == 0) {
      if (refer)
        return make_lval_err(
            "Expecting only one :refer keyword in require vector");
      refer = head->car;
    } else
      return make_lval_err(
          "Expecting only :as and :refer keywords in require vector, "
          "not :%s",
          kw->str);
    head = head->cdr;
  }
  if (refer) {
    if (refer->subtype != VECTOR)
      return make_lval_err(
          "Expecting arg to refer to be a vector in require vector");
  }

  /* printf("NAMESPACE: \n"); */
  /* lval_println(required_namespace_sym); */

  // Create filename of required namespace
  int src_len = _strlen(config->src) + 1;
  char* file_name =
      lalloc_size(src_len + _strlen(required_namespace_sym->str) + 4);
  file_name = _strcpy(file_name, config->src);
  file_name = _strcat(file_name, "/");
  file_name = _strcat(file_name, required_namespace_sym->str);
  file_name = strsubst(file_name, '.', '/');
  file_name = _strcat(file_name, ".clj");
  /* printf("file_name=%s\n", file_name); */

  // If we haven't requireed the env yet we require it now, otherwise we use a
  // cached one.
  Lenv* required_env = (Lenv*)alist_get(state->namespaces, is_eq_str,
                                        required_namespace_sym->str);
  if (!required_env) {
    required_env = require_file(get_root_env(env), file_name);
    state->namespaces = alist_prepend(
        state->namespaces, retain(required_namespace_sym->str), required_env);
  }

  // Add the required namespace to deps as a ns str mapped to its env.
  /* current_namespace->deps = */
  /*     alist_prepend(current_namespace->deps, */
  /*                   retain(required_namespace_sym->str),
   * retain(required_env)); */

  current_namespace->as =
      alist_prepend(current_namespace->as, retain(required_namespace_sym->str),
                    retain(required_namespace_sym->str));

  /* printf("current_namespace->required len: %d\n", */
  /*        list_count(current_namespace->required)); */

  // Add the required namespace to deps as a alternative ns str mapped to the
  // same env.
  if (as) {
    /* printf("AS: %s\n", as); */
    current_namespace->as = alist_prepend(current_namespace->as, retain(as),
                                          retain(required_namespace_sym->str));
  }
  /* printf("current_namespace->required len: %d\n", */
  /*        list_count(current_namespace->required)); */
  /* Cell* first = current_namespace->required->car; */
  /* char* name1 = first->car; */
  /* Lenv* env1 = (Lenv*)first->cdr; */
  /* printf("%s\n", name1); */
  /* env_print(env1); */

  // Add the referred symbols to the refer env of the current namespace
  // Env stack should look like this:
  // builtins <- stdlib <- current namespace (<- lexical envs)
  if (refer) {
    Cell* head = refer->head;
    while (head) {
      Lval* lval_sym = head->car;
      Lval* lval = lenv_get(required_env, lval_sym);
      if (lval->type == LVAL_ERR) {
        return lval;
      }
      /* printf("REQUIRE: adding %s to current_namespace->refer as %s\n", */
      /*        lval_sym->str, required_namespace_sym->str); */
      current_namespace->refer =
          alist_prepend(current_namespace->refer, retain(lval_sym->str),
                        retain(required_namespace_sym->str));
      head = head->cdr;
    }
  }

  /* env_print(required_env); */
  /* printf("as: %s\n", as); */
  /* printf("refer: "); */
  /* lval_println(refer); */
  return make_lval_nil();
}

LispyFn misc_builtins[] = {{"eval", eval_fn},
                           {"print-env", print_env_fn},
                           {"exit", exit_fn},
                           {"load", load_fn},
                           /* {"mpc_load", mpc_load_fn}, */
                           {"read-string", read_string_fn},
                           {"macroexpand", macroexpand_fn},
                           {"macroexpand-1", macroexpand_1_fn},
                           {"compile", compile_fn},
                           {"in-ns", in_ns_fn},
                           {"require", require_fn},
                           {NIL}

};
