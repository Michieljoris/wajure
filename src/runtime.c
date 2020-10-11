#include "runtime.h"

#include <stddef.h>

#include "io.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"
#include "print.h"

Lval* make_lval_wasm_lambda(int fn_table_index,
                            /* int param_count, */
                            /* int has_rest_arg, */
                            int closure, int partials, int partial_count,
                            int fn_call_relay_array) {
  printf(
      "fn_table_index %d, closure: %d, partials %d, fn_call_relay_array %d\n",
      fn_table_index, closure, partials, fn_call_relay_array);
/* printf("*closure: %li\n", *(long*)closure); */
/* Lval* lval = (Lval*)*(long*)closure; */
/* lval_println(lval); */
#ifdef WASM
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){
      .type = LVAL_FUNCTION,
      .subtype = LAMBDA,
      .fn_table_index = fn_table_index,
      .closure = closure,
      .partials = partials,
      .partial_count = partial_count,
      .fn_call_relay_array = fn_call_relay_array
      /* .str = "fn_name:TODO" */
      /* .param_count = param_count, */
      /* .has_rest_arg = has_rest_arg, */
  };
  return lval;
#else
  return NULL;
#endif
}

void dbg(Lval* lval) {
  printf("DBG: Hello!!!\n");
  lval_println(lval);

  printf("releasing:\n");
  printf("lval_list: %d\n", get_ref_count(lval));
  printf("lval_list->data.head: %d\n", get_ref_count(lval->data.head));
  printf("lval_list->data.head->car: %d\n",
         get_ref_count(lval->data.head->car));
  printf("lval_list->data.head->cdr: %d\n",
         get_ref_count(lval->data.head->cdr));
  printf("lval_list->data.head->cdr-car: %d\n",
         get_ref_count(lval->data.head->cdr->car));
  printf("lval_list->data.head->cdr->cdr: %d\n",
         get_ref_count(lval->data.head->cdr->cdr));
  printf("lval_list->data.head->cdr-->cdr->car: %d\n",
         get_ref_count(lval->data.head->cdr->cdr->car));
}

#ifdef WASM
// Used in runtime of lispy compiler
int get_wval_type(Lval* lval) { return lval->type; }
int get_wval_subtype(Lval* lval) { return lval->subtype; }
int get_wval_fn_table_index(Lval* wval) { return wval->fn_table_index; }
/* int get_wval_param_count(Lval* wval) { return wval->param_count; } */
/* int get_wval_min_param_count(Lval* wval) { return wval->param_count - 1; } */
/* int get_wval_has_rest_arg(Lval* wval) { return wval->has_rest_arg; } */
int get_wval_closure(Lval* wval) { return wval->closure; }
int get_wval_partials(Lval* wval) { return wval->partials; }
int get_wval_partial_count(Lval* wval) { return wval->partial_count; }
int get_wval_fn_call_relay_array(Lval* wval) {
  /* char* ptr = (char*)wval->fn_call_relay_array; */
  /* for (int i = 0; i < 21; i++) { */
  /*   printf("%d ", *((char*)(ptr + i))); */
  /* } */
  /* printf("\n"); */
  return wval->fn_call_relay_array;
}
#endif

void bundle_rest_args(int rest_arg_index, Lval** args, int args_count) {
  int rest_arg_length = args_count - rest_arg_index;
  if (rest_arg_length == 0) {
    args[rest_arg_index] = make_lval_nil();
    return;
  }
  Lval* lval_list = make_lval_list();
  Cell* head = NULL;
  int i = args_count - 1;
  while (i >= rest_arg_index) head = list_cons(args[i--], head);
  lval_list->data.head = head;
  args[rest_arg_index] = lval_list;
}

void wval_print(Lval* lval) {
/* lval_println(wval); */
#ifdef WASM
  printf("WVAL---------------------:\n");
  printf("wval pointer: %li\n", (long)lval);
  printf("type: %d %s %lu\n", lval->type,
         lval_type_constant_to_name(lval->type), offsetof(Lval, type));
  printf("subtype: %d %s %lu\n", lval->subtype,
         lval_type_constant_to_name(lval->subtype), offsetof(Lval, subtype));

  printf("head: %d \n", lval->data.head);
  printf("fn_table_index: %d %lu\n", lval->fn_table_index,
         offsetof(Lval, fn_table_index));
  printf("partial_count: %d %lu\n", lval->partial_count,
         offsetof(Lval, partial_count));
  printf("closure: %d %lu\n", lval->closure, offsetof(Lval, closure));
  printf("partials: %d %lu\n", lval->partials, offsetof(Lval, partials));

  printf("fn_call_relay_array: %d %lu\n", lval->fn_call_relay_array,
         offsetof(Lval, fn_call_relay_array));

  char* array = (char*)lval->fn_call_relay_array;
  for (int i = 0; i < 21; i++) {
    printf("%d ", array[i]);
  }
  /* printf("\n"); */
#endif
}

Slot* get_slot_pointer(int* ptr, int i, int data_offset) {
  return (Slot*)(long)(ptr[i] + data_offset);
}

void rewrite_pointers(int data_offset, int data_size, int fn_table_offset) {
  /* printf("data_start: %il\n", data_start); */
  /* printf("data_size: %d\n", data_size); */
  int info_section_size_in_bytes = *(int*)((long)data_offset + data_size - 4);
  int* info_section =
      (int*)((long)data_offset + (data_size - info_section_size_in_bytes - 4));

  // Rewrite lvals
  int* lval_offsets_ptr = (int*)((long)info_section[0] + data_offset);
  int lval_offsets_count = info_section[1];

  /* printf("rewriting lvals:\nlval_offsets_ptr: %li (%li),count: %d\n", */
  /*        (long)info_section[0], (long)lval_offsets_ptr, lval_offsets_count);
   */

  /* for (int i = 0; i < lval_offsets_count; i++) { */
  /*   printf("%d, ", lval_offsets_ptr[i]); */
  /* } */
  /* printf("\n"); */
  for (int i = 0; i < lval_offsets_count; i++) {
    Slot* slot_ptr = get_slot_pointer(lval_offsets_ptr, i, data_offset);
    Lval* lval_ptr = (Lval*)((long)slot_ptr + sizeof(Slot));
    /* printf("lval_ptr: %li\n", (long)lval_ptr - data_offset); */
    /* wval_print(lval_ptr); */
    slot_ptr->data_p = slot_ptr->data_p + data_offset;

    if (lval_ptr->subtype != NUMBER &&
        lval_ptr->data.str)  // this also adds offset to data.head, as it should
      lval_ptr->data.str += data_offset;
      /* if (lval_ptr->data.head) */
      /*   lval_ptr->data.head = (Cell*)((char*)lval_ptr->data.head +
       * data_offset); */

#ifdef WASM
    lval_ptr->fn_table_index += fn_table_offset;
#endif

    if (lval_ptr->closure) lval_ptr->closure += data_offset;
    if (lval_ptr->partials) lval_ptr->partials += data_offset;
#ifdef WASM
    lval_ptr->fn_call_relay_array += data_offset;
#endif

#ifdef WASM
    int* partials = (int*)((long)lval_ptr->partials);
    for (int i = 0; i < lval_ptr->partial_count; i++) {
      partials[i] += data_offset;
    }
#endif

    /* wval_print(lval_ptr); */
  }

  // Rewrite cells
  int* cell_offsets_ptr = (int*)((long)info_section[2] + data_offset);
  int cell_offsets_count = info_section[3];

  /* printf("rewriting cells:\ncell_offsets_ptr: %li (%li),count: %d\n", */
  /*        (long)info_section[2], (long)cell_offsets_ptr, cell_offsets_count);
   */

  for (int i = 0; i < cell_offsets_count; i++) {
    /* printf("slot_ptr %li\n", (long)((int*)cell_offsets_ptr)[i]); */
    Slot* slot_ptr = get_slot_pointer(cell_offsets_ptr, i, data_offset);
    Cell* cell_ptr = (Cell*)((long)slot_ptr + sizeof(Slot));
    /* printf("rewriting: slot: %li cell: %li: \n", (long)slot_ptr -
     * data_offset, */
    /*        (long)cell_ptr - data_offset); */
    /* printf("car: %li, cdr: %li\n", (long)cell_ptr->car, (long)cell_ptr->cdr);
     */
    /* printf("slot_ptr->data_p %li\n", (long)slot_ptr->data_p); */
    slot_ptr->data_p = slot_ptr->data_p + data_offset;
    if (cell_ptr->car) cell_ptr->car += data_offset;
    if (cell_ptr->cdr)
      cell_ptr->cdr = (Cell*)((char*)cell_ptr->cdr + data_offset);

    /* printf("car: %li, cdr: %li\n", (long)cell_ptr->car, (long)cell_ptr->cdr);
     */
  }

  // Rewrite wval_fns
  /*   char* wval_fn_offsets_ptr = (char*)(long)info_section[4]; */

  /*   /\* printf("wval_fn_offsets_ptr:  %li\n", (long)wval_fn_offsets_ptr); *\/
   */
  /*   wval_fn_offsets_ptr += data_offset; */
  /*   int wval_fn_offsets_count = info_section[5]; */

  /*   printf("rewriting wval_fn:\nwval_fn_offsets_ptr: %li\ncount: %d %d\n", */
  /*          (long)wval_fn_offsets_ptr, wval_fn_offsets_count, */
  /*          wval_fn_offsets_ptr[0]); */

  /*   for (int i = 0; i < wval_fn_offsets_count; i++) { */
  /*     Slot* slot_ptr = */
  /*         (Slot*)(long)(((int*)wval_fn_offsets_ptr)[i] + data_offset); */
  /*     Lval* wval_fn_ptr = (Lval*)(long)(((int*)wval_fn_offsets_ptr)[i] + */
  /*                                       data_offset + sizeof(Slot)); */

  /*     wval_print(wval_fn_ptr); */
  /*     slot_ptr->data_p = slot_ptr->data_p + data_offset; */
  /* #ifdef WASM */
  /*     wval_fn_ptr->fn_table_index += fn_table_offset; */
  /* #endif */

  /*     if (wval_fn_ptr->closure) wval_fn_ptr->closure += data_offset; */
  /*     if (wval_fn_ptr->partials) wval_fn_ptr->partials += data_offset; */
  /* #ifdef WASM */
  /*     wval_fn_ptr->fn_call_relay_array += data_offset; */
  /* #endif */

  /* #ifdef WASM */
  /*     int* partials = (int*)((long)wval_fn_ptr->partials); */
  /*     for (int i = 0; i < wval_fn_ptr->partial_count; i++) { */
  /*       partials[i] += data_offset; */
  /*       /\* printf("partial[%d] = %d\n", i, partials[i]); *\/ */
  /*     } */
  /* #endif */

  /*     wval_print(wval_fn_ptr); */
  /* printf("wval_fn->partials %d\n", wval_fn_ptr->partials); */
  /* printf("wval_fn->partial_count %d\n", wval_fn_ptr->partial_count); */
  /* printf("wval_fn_ptr->fn_table_index: %d\n", wval_fn_ptr->fn_table_index);
   */
  /* } */
}

/* int is_falsy(Lval* lval) { */
/*   return lval->subtype == LFALSE || lval->subtype == LNIL; */
/* } */

/* Lval** get_wval_closure(WvalFun* wval) { return wval->closure; } */

/* int check_args_count(int param_count, int args_count, int has_rest_arg) { */
/*   if (has_rest_arg != 1) { */
/*     if (!has_rest_arg) { */
/*       if (args_count < param_count) return TOO_FEW_ARGS; */
/*       if (args_count > param_count) return TOO_MANY_ARGS; */
/*     } else { */
/*       if (args_count < param_count - 1) return TOO_FEW_ARGS; */
/*     } */
/*   } */
/*   return ARGS_MATCH_PARAMS; */
/* } */
