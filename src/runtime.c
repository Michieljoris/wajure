#include "runtime.h"

#include <stddef.h>

#include "io.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"

Lval* make_lval_wasm_lambda(int fn_table_index, int param_count,
                            int has_rest_arg, Lval** closure, Lval** partials,
                            int partial_count) {
  Wval* wval = lalloc_type(LVAL);
  *wval = (Wval){.type = LVAL_WASM_LAMBDA,
                 .subtype = -1,
                 .fn_table_index = fn_table_index,
                 .param_count = param_count,
                 .has_rest_arg = has_rest_arg,
                 .closure = closure,
                 .partials = partials,
                 .partial_count = partial_count};
  return (Lval*)wval;
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

void wval_print(Wval* wval) {
  printf("WVAL:\n");
  printf("wval pointer: %li\n", (long)wval);
  printf("type: %d %lu\n", wval->type, offsetof(Wval, type));
  printf("subtype: %d %lu\n", wval->subtype, offsetof(Wval, subtype));
  printf("fn_table_index: %d %lu\n", wval->fn_table_index,
         offsetof(Wval, fn_table_index));
  printf("param_count: %d %lu\n", wval->param_count,
         offsetof(Wval, param_count));

  printf("has_rest_arg: %d %lu\n", wval->has_rest_arg,
         offsetof(Wval, has_rest_arg));
  printf("partial_count: %d %lu\n", wval->partial_count,
         offsetof(Wval, partial_count));
  printf("closure: %li %lu\n", (long)wval->closure, offsetof(Wval, closure));
  printf("partials: %li %lu\n", (long)wval->partials, offsetof(Wval, partials));
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

Lval** get_wval_closure(Wval* wval) { return wval->closure; }
