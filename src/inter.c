#include "inter.h"

#include "compile.h"
#include "datafy.h"
#include "lib.h"
#include "list.h"
#include "ltypes.h"
#include "native.h"
#include "print.h"
#include "wasm.h"
#include "wasm_util.h"

// Slot offsets
#define ref_count_offset 0
#define data_p_offset 12  // bytes
#define slot_type_size data_p_offset + 4

// Cell offsets
#define wcell_size slot_type_size + cell_type_size
#define cell_hash_offset slot_type_size + 0
#define car_offset slot_type_size + 4
#define cdr_offset slot_type_size + 8
#define cell_type_size cdr_offset + 4

// Lval offsets
#define type_offset slot_type_size + 0     // char
#define subtype_offset slot_type_size + 1  // char
#define d_offset slot_type_size + 4        // int
#define hash_offset slot_type_size + 8     // int

#define fn_table_index_offset slot_type_size + 12  // short
#define partial_count_offset slot_type_size + 14   // short
#define closure_offset slot_type_size + 16         // int
#define partials_offset slot_type_size + 20        // int
#define fn_call_relay_offset slot_type_size + 24   // int
#define lval_type_size fn_call_relay_offset + 4
/* #define str_offset 4  // 20 */

void add_to_offset_list(int** offsets, int* count, int* allocated, int offset) {
  if (*count >= *allocated) {
    *allocated += 100;
    *offsets = realloc(*offsets, *allocated * sizeof(int));
  }
  (*offsets)[*count] = offset;
  (*count)++;
}

// pointers are 32 bits in our wasm, max mem reachable mem is about 4gb
int ptr_to_int(void* p) { return (int)((long)p & 0xFFFFFFFF); }

char* make_data_cell(Wasm* wasm, Cell* cell) {
  char* data_cell = calloc(1, wcell_size);

  *(int*)(data_cell + ref_count_offset) = 1;
  *(int*)(data_cell + hash_offset) = cell->hash;
  return data_cell;
}

CResult inter_data_cell(Wasm* wasm, char* data_cell) {
  *(int*)(data_cell + data_p_offset) =
      wasm->__data_end + wasm->data_offset + slot_type_size;
  int offset = add_bytes_to_data(wasm, data_cell, wcell_size);
  add_to_offset_list(&wasm->cell_offsets, &wasm->cell_offsets_count,
                     &wasm->cell_offsets_allocated, offset);
  /* printf("data cell: \n"); */
  /* for (int i = 0; i < 7; i++) { */
  /*   int* c = (int*)data_cell; */
  /*   printf("%d: %d\n", i, c[i]); */
  /* } */
  free(data_cell);
  int wcell_ptr = wasm->__data_end + offset + slot_type_size;

  CResult ret = {/* .ber = make_ptr(wasm, wcell_ptr), */
                 .data_offset = wcell_ptr};
  return ret;
}

int inter_data_lval(Wasm* wasm, char* data_lval) {
  *(int*)(data_lval + data_p_offset) =
      wasm->__data_end + wasm->data_offset + slot_type_size;
  int offset = add_bytes_to_data(wasm, data_lval, lval_type_size);
  add_to_offset_list(&wasm->lval_offsets, &wasm->lval_offsets_count,
                     &wasm->lval_offsets_allocated, offset);
  free(data_lval);
  int wval_ptr = wasm->__data_end + offset + slot_type_size;
  return wval_ptr;
}

// Primitive types (int, true, false, nil, str)
int inter_lval(Wasm* wasm, Lval* lval) {
  char* data_lval = make_data_lval(wasm, lval, 0, FCRA_NOT_A_FN_OFFSET);
  return inter_data_lval(wasm, data_lval);
}

int inter_list(Wasm* wasm, Lval* lval) {
  Cell* head = lval->data.head;
  int count = list_count(head);
  char* data_cells[count];
  int i = 0;
  while (head) {
    int v_ptr = datafy_lval(wasm, head->car).data_offset;
    char* data_cell = make_data_cell(wasm, head);
    *(int*)(data_cell + car_offset) = v_ptr;
    data_cells[i++] = data_cell;
    head = head->cdr;
  }
  int cdr = 0;
  int last_cdr = 0;
  while (i--) {
    char* data_cell = data_cells[i];
    *(int*)(data_cell + cdr_offset) = cdr;
    cdr = inter_data_cell(wasm, (char*)data_cell).data_offset;
    last_cdr = cdr;
  }
  char* data_list = make_data_lval(wasm, lval, 0, FCRA_NOT_A_FN_OFFSET);

  *(int*)(data_list + d_offset) = cdr;
  /* printf("cdr: %d\n", cdr); */
  int ret = inter_data_lval(wasm, data_list);
  return ret;
}

int inter_lval_str_type(Wasm* wasm, Cell** pool, Lval* lval) {
  int* ret = (int*)alist_get(*pool, is_eq_str, lval->data.str);
  if (!ret) {
    ret = malloc(sizeof(CResult));
    *ret = inter_lval(wasm, lval);
    *pool = alist_prepend(*pool, lval->data.str, ret);
  }
  return *ret;
}

char* make_data_lval(Wasm* wasm, Lval* lval, int fn_table_index,
                     int fcra_offset) {
  /* int type = lval ? lval->type : LVAL_FUNCTION; */
  /* printf( */
  /*     "make_data_lval type: %s, fn_table_index: %d param_count: " */
  /*     "%d " */
  /*     "has_rest_arg: %d\n", */
  /*     lval_type_constant_to_name(type), fn_table_index, param_count, */
  /*     has_rest_arg); */
  char* data_lval = calloc(1, lval_type_size);
  long p = (long)data_lval;
  /* int string_offset = 0; */
  /* if (lval->str) string_offset = add_string_to_data(wasm, lval->str); */

  *(int*)(p + ref_count_offset) = 1;

  if (lval) {
    *(char*)(p + type_offset) = lval->type;
    *(char*)(p + subtype_offset) = lval->subtype;

    if (lval->subtype == NUMBER)
      *(int*)(p + d_offset) = lval->data.num;
    else if (lval->type == LVAL_COLLECTION)
      *(Cell**)(p + d_offset) = lval->data.head;
    else if (lval->data.str) {
      // string, symbol, keyword, fn_name or regex
      int string_offset = add_string_to_data(wasm, lval->data.str);
      *(int*)(p + d_offset) = wasm->__data_end + string_offset;
    }

    *(int*)(p + hash_offset) = lval->hash;
  } else {
    *(char*)(p + type_offset) = LVAL_FUNCTION;
    *(char*)(p + subtype_offset) = LAMBDA;
  }

  // TODO: obsolete?, using fcra!!!
  *(short*)(p + fn_table_index_offset) = fn_table_index;

  if (lval) {
    int partial_count = list_count(lval->partials);
    *(short*)(p + partial_count_offset) = partial_count;

    Cell* head = lval->partials;
    int partial_ptrs[partial_count];
    int i = 0;
    while (head) {
      Lval* lval = head->car;
      CResult result = datafy_lval(wasm, lval);
      partial_ptrs[i++] = result.data_offset;
      head = head->cdr;
    }
    if (partial_count) {
      int data_ptr =
          add_bytes_to_data(wasm, (char*)partial_ptrs, 4 * partial_count);
      *(int*)(p + partials_offset) = data_ptr;
    }
  }
  *(int*)(p + fn_call_relay_offset) = fcra_offset;

  return data_lval;
}

void inter_rewrite_info(Wasm* wasm) {
  int lval_offsets_ptr = add_bytes_to_data(wasm, (char*)wasm->lval_offsets,
                                           wasm->lval_offsets_count * 4);
  /* printf("lval_offset_ptr %d, count: %d\n", lval_offsets_ptr, */
  /*        wasm->lval_offsets_count); */

  /* for (int i = 0; i < wasm->lval_offsets_count; i++) { */
  /*   printf("%d, ", wasm->lval_offsets[i]); */
  /* } */

  int cell_offsets_ptr = add_bytes_to_data(wasm, (char*)wasm->cell_offsets,
                                           wasm->cell_offsets_count * 4);
  /* printf("cell_offset_ptr %d, count %d\n", cell_offsets_ptr, */
  /*        wasm->cell_offsets_count); */
  /* for (int i = 0; i < wasm->cell_offsets_count; i++) { */
  /*   printf("%d, ", wasm->cell_offsets[i]); */
  /* } */

  /* int wval_fn_offsets_ptr = add_bytes_to_data( */
  /*     wasm, (char*)wasm->wval_fn_offsets, wasm->wval_fn_offsets_count * 4);
   */

  /* printf("wval_fn_offset_ptr %d, count: %d\n", wval_fn_offsets_ptr, */
  /*        wasm->wval_fn_offsets_count); */
  int info_section[] = {
      lval_offsets_ptr, wasm->lval_offsets_count, cell_offsets_ptr,
      wasm->cell_offsets_count,
      /* wval_fn_offsets_ptr, wasm->wval_fn_offsets_count, */
  };

  /* printf("sizeof info_section %li\n", sizeof(info_section)); */
  /* printf("info_section[0]: %d\n", info_section[0]); */
  /* printf("info_section[1]: %d\n", info_section[1]); */
  /* printf("info_section[2]: %d\n", info_section[2]); */
  /* printf("wasm->data_offset %d\n", wasm->data_offset); */
  add_bytes_to_data(wasm, (char*)info_section, sizeof(info_section));
  /* printf("wasm->data_offset %d\n", wasm->data_offset); */
  int info_section_size = sizeof(info_section);
  /* printf("info_section_size %d\n", info_section_size); */
  add_bytes_to_data(wasm, (char*)&info_section_size, sizeof(int));
  /* printf("wasm->data_offset %d\n", wasm->data_offset); */
}

/* CResult inter_cell(Wasm* wasm, Cell* cell) { */
/*   int* data_cell = make_data_cell(wasm, cell); */
/*   return inter_data_cell(wasm, data_cell); */
/* } */

/* char* make_data_lval(Wasm* wasm, Lval* lval) { */
/*   int string_offset = 0; */
/*   if (lval->subtype != NUMBER && lval->data.str) */
/*     string_offset = add_string_to_data(wasm, lval->data.str); */

/*   char* data_lval = calloc(1, lval_type_size); */
/*   long p = (long)data_lval; */
/*   /\* int string_offset = 0; *\/ */
/*   /\* if (lval->str) string_offset = add_string_to_data(wasm, lval->str); *\/
 */

/*   *(int*)(p + ref_count_offset) = 1; */

/*   *(char*)(p + type_offset) = lval->type; */
/*   *(char*)(p + subtype_offset) = lval->subtype; */

/*   if (lval->subtype == NUMBER) */
/*     *(int*)(p + d_offset) = lval->data.num; */
/*   else { */
/*     if (lval->data.str) */
/*       *(int*)(p + d_offset) = wasm->__data_end + string_offset; */
/*   } */
/*   *(int*)(p + hash_offset) = lval->hash; */

/*   // We really need to set this pointer at the end since we set the datap_p
 */
/*   // relative to wasm->data_offset, and we have just done a
 * add_bytes_to_data. */
/*   *(int*)(p + data_p_offset) = */
/*       wasm->__data_end + wasm->data_offset + slot_type_size; */

/*   return data_lval; */
/* } */

/* int inter_data_lval_wasm_lambda(Wasm* wasm, char* data_lval) { */
/*   int offset = add_bytes_to_data(wasm, data_lval, lval_type_size); */
/*   add_to_offset_list(&wasm->wval_fn_offsets, &wasm->wval_fn_offsets_count, */
/*                      &wasm->wval_fn_offsets_allocated, offset); */
/*   int wval_ptr = wasm->__data_end + offset + slot_type_size; */
/*   printf("interred wasm_lambda: wval_ptr = %d\n", wval_ptr); */
/*   return wval_ptr; */
/* } */
