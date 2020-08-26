#include "runtime.h"

#include <stddef.h>

#include "io.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"

WvalFun* make_lval_wasm_lambda(int fn_table_index, int param_count,
                               int has_rest_arg, int closure, int partials,
                               int partial_count) {
  WvalFun* wval = lalloc_type(LVAL);
  *wval = (WvalFun){.type = LVAL_WASM_LAMBDA,
                    .subtype = -1,
                    .fn_table_index = fn_table_index,
                    .param_count = param_count,
                    .has_rest_arg = has_rest_arg,
                    .closure = closure,
                    .partials = partials,
                    .partial_count = partial_count,
                    /* .str = "fn_name:TODO" */};
  return wval;
}

// Used in runtime of lispy compiler
void init_rest_args(Lval** lval_array, int rest_arg_length) {
  /* printf("Args_count: %d\n", rest_arg_length); */
  Lval* lval_list = make_lval_list();
  Cell* head = NULL;
  int i = 0;
  while (i < rest_arg_length) head = list_cons(lval_array[i++], head);
  lval_list->head = head;
  /* lval_println(lval_list); */
  lval_array[rest_arg_length - 1] = lval_list;
}

void wval_print(WvalFun* wval) {
  printf("WVAL:\n");
  printf("wval pointer: %li\n", (long)wval);
  printf("type: %d %lu\n", wval->type, offsetof(WvalFun, type));
  printf("subtype: %d %lu\n", wval->subtype, offsetof(WvalFun, subtype));
  printf("fn_table_index: %d %lu\n", wval->fn_table_index,
         offsetof(WvalFun, fn_table_index));
  printf("param_count: %d %lu\n", wval->param_count,
         offsetof(WvalFun, param_count));

  printf("has_rest_arg: %d %lu\n", wval->has_rest_arg,
         offsetof(WvalFun, has_rest_arg));
  printf("partial_count: %d %lu\n", wval->partial_count,
         offsetof(WvalFun, partial_count));
  printf("closure: %li %lu\n", (long)wval->closure, offsetof(WvalFun, closure));
  printf("partials: %li %lu\n", (long)wval->partials,
         offsetof(WvalFun, partials));
}

int check_args_count(int param_count, int args_count, int has_rest_arg) {
  if (!has_rest_arg) {
    if (args_count < param_count) return TOO_FEW_ARGS;
    if (args_count > param_count) return TOO_MANY_ARGS;
  } else {
    if (args_count < param_count - 1) return TOO_FEW_ARGS;
  }
  return ARGS_MATCH_PARAMS;
}

/* Lval** get_wval_closure(WvalFun* wval) { return wval->closure; } */
