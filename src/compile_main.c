#include <sys/stat.h>

#include "builtin.h"
#include "compile.h"
#include "io.h"
#include "lispy_mempool.h"
#include "list.h"
#include "namespace.h"
#include "print.h"
#include "run.h"
#include "state.h"

int is_src_newer(char* src, char* wasm) {
  struct stat src_stat;
  if (stat(src, &src_stat) < 0) {
    printf("Trying to stat src %s, but an error was returned\n", src);
    exit(1);
  };
  struct stat wasm_stat;

  int ret =
      stat(wasm, &wasm_stat) < 0 ? 1 : src_stat.st_mtime > wasm_stat.st_mtime;
  /* printf("comparing %s and %s %d\n", src, wasm, ret); */
  return ret;
}

void print_k(void* pair) {
  printf("%s ", (char*)((Cell*)pair)->car);
  /* printf(": "); */
  /* lval_print((Lval*)((Cell*)pair)->cdr); */
}

void print_alist(Cell* alist) { list_print(alist, print_k, ""); }

void mark_dependants(Namespace* ns) {
  Cell* cell = ns->dependants;
  if (!cell || !cell->car) return;
  while (cell) {
    Cell* pair = cell->car;
    Namespace* dependant_ns = (Namespace*)pair->cdr;
    if (!dependant_ns->compile) mark_dependants(dependant_ns);
    dependant_ns->compile = 1;
    cell = cell->cdr;
  }
}

void mark(Namespace* ns);

void mark_required(Namespace* ns) {
  Cell* cell = ns->required;
  if (!cell || !cell->car) return;
  while (cell) {
    Cell* pair = cell->car;
    Namespace* required_ns = (Namespace*)pair->cdr;
    mark(required_ns);
    cell = cell->cdr;
  }
}

void mark(Namespace* ns) {
  char* namespace_str = ns->namespace;
  char* src = ns_to_src(namespace_str);
  char* wasm = ns_to_wasm(namespace_str);
  if (!ns->compile && is_src_newer(src, wasm)) {
    ns->compile = 1;
    mark_dependants(ns);
  }
  mark_required(ns);
  release(src);
  release(wasm);
}

int walk_namespaces(void f(Namespace*)) {
  Cell* cell = state->namespaces;
  if (!cell || !cell->car) return 1;
  while (cell) {
    Cell* pair = cell->car;
    Namespace* ns = (Namespace*)pair->cdr;
    f(ns);
    cell = cell->cdr;
  }
  return 1;
}

void p_info(Namespace* ns) {
  printf("%s dirty?: %s\n", ns->namespace, ns->compile ? "yes" : "no");
  print_alist(ns->dependants);
}

// Not the best solution, better would be to separate compiler data from lvals
// in a new datastructure, and just wipe that on every compile.
void reset_lval(Lval* lval) {
  lval->lval_ptr = 0;
  lval->fn_table_index = 0;
  lval->context = NULL;
  switch (lval->group) {
    case LVAL_COLLECTION: {
      switch (lval->type) {
        case LIST:
        case VECTOR:;
          Cell* cell = lval->data.head;
          while (cell) {
            lval = (Lval*)cell->car;
            reset_lval(lval);
            cell = cell->cdr;
          }
          break;
        case MAP:
          break;
        case SET:
          break;
      }
    }
    case LVAL_FUNCTION: {
      Cell* cell = lval->partials;
      while (cell) {
        reset_lval(cell->car);
        cell = cell->cdr;
      }
    }
    default:;
  }
}

void maybe_compile(Namespace* ns) {
  /* ns->compile = 1; */
  if (1 || ns->compile) {
    compile(ns);
    /* printf("maybe compile\n"); */
    // Reset any values set on lvals.
    Cell* cell = ns->env->kv;
    while (cell) {
      Cell* pair = cell->car;
      Lval* lval = (Lval*)pair->cdr;
      /* printf("%d: ", lval->wval_ptr); */
      /* lval_println(lval); */
      reset_lval(lval);
      cell = cell->cdr;
    }
  }
}

void unmark(Namespace* ns) { ns->compile = 0; }

int compile_main() {
  init_wajure();

  state->mark_deps = 0;
  if (load_main() == 1) exit(1);

  register_c_fns();
  register_native_fns();
  assign_offsets_to_builtins();

  Namespace* main_ns = get_namespace(config->main);
  walk_namespaces(unmark);
  mark(main_ns);
  main_ns->compile = 1;  // for debugging compiling
  /* Namespace* foo_core_ns = get_namespace("foo.core"); */
  /* foo_core_ns->compile = 1; */
  walk_namespaces(p_info);

  printf("\n");

  state->mark_deps = 1;
  walk_namespaces(maybe_compile);
  printf("----------------------\n");

  destroy_wajure();
  /* printf("\nRequiring: \n"); */
  /* print_alist(ns->required); */
  /* printf("Dependants: \n"); */
  /* print_alist(ns->dependants); */
  /* char** deps = get_module_deps(main_ns->namespace); */
  /* printf("deps: %s\n", deps[0]); */
  /* print_alist(main_ns->as); */

  return 1;
}

/* char** get_module_deps(char* namespace_str) { */
/*   char* wasm_file_name = ns_to_wasm(namespace_str); */
/*   char* node_command = "node custom.js"; */
/*   int size = _strlen(wasm_file_name) + _strlen(node_command); */
/*   char* command = malloc(size); */
/*   sprintf(command, "%s %s", node_command, wasm_file_name); */
/*   release(wasm_file_name); */
/*   FILE* f = popen(command, "r"); */
/*   int line_size = 1028; */
/*   int dep_lines_allocated = 100; */
/*   char** deps = malloc(dep_lines_allocated); */
/*   int i = 0; */
/*   char* line = calloc(line_size, 1); */
/*   while (fgets(line, line_size, f) != NULL) { */
/*     line = realloc(line, _strlen(line) + 1); */
/*     if (*line == '\n') { */
/*       free(line); */
/*       break; */
/*     } */
/*     deps[i++] = line; */
/*     if (i == dep_lines_allocated) { */
/*       dep_lines_allocated += 100; */
/*       deps = realloc(deps, dep_lines_allocated); */
/*     } */
/*     line = calloc(line_size, 1); */
/*   } */
/*   pclose(f); */
/*   return deps; */
/* } */
