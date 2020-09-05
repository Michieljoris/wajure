#include "runtime.h"

#include <stddef.h>

#include "io.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"
#include "print.h"

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

void rewrite_pointers(int data_offset, int data_size, int fn_table_offset) {
  /* printf("data_start: %il\n", data_start); */
  /* printf("data_size: %d\n", data_size); */
  int info_section_size_in_bytes = *(int*)((long)data_offset + data_size - 4);
  int* info_section =
      (int*)((long)data_offset + (data_size - info_section_size_in_bytes - 4));

  /* int info_section_size = info_section_size_in_bytes / 4; */

  char* lval_offsets_ptr = (char*)(long)info_section[0] + data_offset;
  int lval_offsets_count = info_section[1];
  for (int i = 0; i < lval_offsets_count; i++) {
    Slot* slot_ptr = (Slot*)(long)(((int*)lval_offsets_ptr)[i] + data_offset);
    Lval* lval_ptr =
        (Lval*)(long)(((int*)lval_offsets_ptr)[i] + data_offset + sizeof(Slot));
    slot_ptr->data_p = slot_ptr->data_p + data_offset;

    if (lval_ptr->str) lval_ptr->str += data_offset;
    if (lval_ptr->head)
      lval_ptr->head = (Cell*)((char*)lval_ptr->head + data_offset);
  }

  char* cell_offsets_ptr = (char*)(long)info_section[2] + data_offset;
  int cell_offsets_count = info_section[3];
  /* printf("cell 0: %li %d %li\n", (long)cell_offsets_ptr, cell_offsets_count,
   */
  /*        (long)((int*)cell_offsets_ptr)[0]); */

  for (int i = 0; i < cell_offsets_count; i++) {
    Slot* slot_ptr = (Slot*)(long)(((int*)cell_offsets_ptr)[i] + data_offset);
    Cell* cell_ptr =
        (Cell*)(long)(((int*)cell_offsets_ptr)[i] + data_offset + sizeof(Slot));
    slot_ptr->data_p = slot_ptr->data_p + data_offset;
    if (cell_ptr->car) cell_ptr->car += data_offset;
    if (cell_ptr->cdr)
      cell_ptr->cdr = (Cell*)((char*)cell_ptr->cdr + data_offset);
  }

  char* wval_fn_offsets_ptr = (char*)(long)info_section[4];

  /* printf("wval_fn_offsets_ptr:  %li\n", (long)wval_fn_offsets_ptr); */
  wval_fn_offsets_ptr += data_offset;
  int wval_fn_offsets_count = info_section[5];

  /* printf("rewriting wval_fn:\nwval_fn_offsets_ptr: %li\ncount: %d %d\n", */
  /*        (long)wval_fn_offsets_ptr, wval_fn_offsets_count, */
  /*        wval_fn_offsets_ptr[0]); */

  for (int i = 0; i < wval_fn_offsets_count; i++) {
    Slot* slot_ptr =
        (Slot*)(long)(((int*)wval_fn_offsets_ptr)[i] + data_offset);
    WvalFun* wval_fn_ptr = (WvalFun*)(long)(((int*)wval_fn_offsets_ptr)[i] +
                                            data_offset + sizeof(Slot));
    slot_ptr->data_p = slot_ptr->data_p + data_offset;
    wval_fn_ptr->fn_table_index += fn_table_offset;
    /* printf("wval_fn_ptr->fn_table_index: %d\n", wval_fn_ptr->fn_table_index);
     */
  }
}
/* int is_falsy(Lval* lval) { */
/*   return lval->subtype == LFALSE || lval->subtype == LNIL; */
/* } */

/* Lval** get_wval_closure(WvalFun* wval) { return wval->closure; } */