#include "namespace.h"

#include "env.h"
#include "fns.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"
#include "state.h"

Namespace* make_or_get_namespace(char* namespace_str) {
  Namespace* ns = alist_get(state->namespaces, is_eq_str, namespace_str);

  if (!ns) {
    ns = lalloc_type(NAMESPACE);
    *ns = (Namespace){.namespace = retain(namespace_str), .env = lenv_new()};
    state->namespaces =
        alist_prepend(state->namespaces, retain(namespace_str), ns);
  }
  return ns;
}

char* ns_to_file_name(char* ns_str) {
  char* file_name = lalloc_size(_strlen(config->src) + 1 + _strlen(ns_str) +
                                _strlen("/.clj") + 1);
  file_name = _strcpy(file_name, config->src);
  file_name = _strcat(file_name, "/");
  file_name = _strcat(file_name, ns_str);
  file_name = strsubst(file_name, '.', '/');
  file_name = strsubst(file_name, '-', '_');
  file_name = _strcat(file_name, ".clj");
  return file_name;
}

Namespace* get_stdlib_ns() {
  return alist_get(state->namespaces, is_eq_str, config->stdlib);
}

Lenv* get_stdlib_env() {
  Namespace* stdlib_ns = get_stdlib_ns();
  return stdlib_ns ? stdlib_ns->env : NULL;
}

Lval* make_lval_ns_symbol() {
  scoped char* str = lalloc_size(5);
  _strcpy(str, "*ns*");
  return make_lval_sym(str);
}

void set_current_ns(Namespace* ns) { state->current_ns = ns; }

Namespace* get_current_ns() {
  return state->current_ns;
  /* Lenv* stdlib_env = get_stdlib_env(); */
  /* if (stdlib_env) { */
  /*   scoped Lval* lval_ns_symbol = make_lval_ns_symbol(); */
  /*   scoped Lval* lval_ns = lenv_get(stdlib_env, lval_ns_symbol); */
  /*   if (lval_ns) return (Namespace*)lval_ns->head; */
  /* } */
  /* return get_namespace("user"); */
}

Namespace* get_namespace(char* namespace_str) {
  return alist_get(state->namespaces, is_eq_str, namespace_str);
}

Namespace* get_ns_for_namespaced_symbol(Lval* lval_symbol,
                                        char* namespace_or_alias) {
  Namespace* current_namespace = get_current_ns();
  char* namespace_str =
      alist_get(current_namespace->as, is_eq_str, namespace_or_alias);

  return get_namespace(namespace_str);
}

Namespace* get_ns_for_referred_symbol(Lval* lval_symbol) {
  Namespace* current_namespace = get_current_ns();
  // Is symbol refering to a value in another namespace?
  char* namespace_str =
      alist_get(current_namespace->refer, is_eq_str, lval_symbol->str);
  return namespace_str ? get_namespace(namespace_str) : NULL;
  /* Namespace* ns = get_namespace(namespace_str); */
  /* return ns ? ns->env : NULL; */
}

Namespace* install_ns(char* str) {
  scoped char* ns_str = lalloc_size(_strlen(str) + 1);
  _strcpy(ns_str, str);
  Namespace* ns = make_or_get_namespace(ns_str);
  return ns;
}

void install_builtins() {
  Lenv* builtins_env = lenv_new();
  lenv_add_builtin_fns(builtins_env);
  state->builtins_env = builtins_env;
}

Namespace* install_stdlib() {
  Namespace* stdlib_ns = NULL;
  if (config->stdlib) {
    scoped Lval* result = require_ns(NULL, config->stdlib);
    if (result->type == LVAL_ERR) {
      lval_println(result);
      /* abort(); */
    }
    stdlib_ns = get_namespace(config->stdlib);
    if (!stdlib_ns) {
      printf(
          "Loading %s didn't create the corresponding namespace. Add (in-ns "
          "'%s) to the top of the file.\n",
          config->stdlib, config->stdlib);
      /* abort(); */
    }
    state->stdlib_ns = stdlib_ns;
  }
  return stdlib_ns;
}
