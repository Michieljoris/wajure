#include "inter.h"

#include "compile.h"
#include "datafy.h"
#include "lib.h"
#include "list.h"
#include "ltypes.h"
#include "wasm.h"
#include "wasm_util.h"

#define slot_type_size 4 * 4
#define cell_type_size 3 * 4
#define ref_count_offset 0
#define data_p_offset 3

#define wcell_size slot_type_size + cell_type_size
#define cell_hash_offset 4
#define car_offset 5
#define cdr_offset 6

// pointers are 32 bits in our wasm, max mem reachable mem is about 4gb
int ptr_to_int(void* p) { return (int)((long)p & 0xFFFFFFFF); }

int* make_data_cell(Wasm* wasm, Cell* cell) {
  int* data_cell = calloc(1, wcell_size);

  data_cell[ref_count_offset] = 1;
  data_cell[data_p_offset] =
      wasm->__data_end + wasm->data_offset + slot_type_size;
  data_cell[car_offset] = 0;
  data_cell[cdr_offset] = 0;
  data_cell[cell_hash_offset] = cell->hash;
  return data_cell;
}

CResult inter_data_cell(Wasm* wasm, int* data_cell) {
  int offset = add_bytes_to_data(wasm, (char*)data_cell, wcell_size);
  free(data_cell);
  int wcell_ptr = wasm->__data_end + offset + slot_type_size;
  CResult ret = {.ber = make_int32(wasm->module, wcell_ptr),
                 .wasm_ptr = wcell_ptr};
  return ret;
}

CResult inter_cell(Wasm* wasm, Cell* cell) {
  int* data_cell = make_data_cell(wasm, cell);
  return inter_data_cell(wasm, data_cell);
}

#define lval_type_size 5 * 4  // type and subtype are in 4 bytes
#define wval_size slot_type_size + lval_type_size

#define type_offset 4

#define num_offset 5
#define str_offset 6
#define head_offset 7
#define hash_offset 8

int* make_data_lval(Wasm* wasm, Lval* lval) {
  int* data_lval = calloc(1, wval_size);
  int string_offset = 0;
  if (lval->str) string_offset = add_string_to_data(wasm, lval->str);

  data_lval[ref_count_offset] = 1;
  data_lval[data_p_offset] =
      wasm->__data_end + wasm->data_offset + slot_type_size;
  data_lval[type_offset] = lval->type | lval->subtype << 8;
  data_lval[num_offset] = lval->num;
  data_lval[str_offset] = wasm->__data_end + string_offset;
  data_lval[head_offset] = 0;
  data_lval[hash_offset] = lval->hash;

  return data_lval;
}

CResult inter_data_lval(Wasm* wasm, int* data_lval) {
  int offset = add_bytes_to_data(wasm, (char*)data_lval, wval_size);
  free(data_lval);
  int wval_ptr = wasm->__data_end + offset + slot_type_size;
  CResult ret = {.ber = make_int32(wasm->module, wval_ptr),
                 .wasm_ptr = wval_ptr};
  return ret;
}

// Primitive types (int, true, false, nil, str)
CResult inter_lval(Wasm* wasm, Lval* lval) {
  int* data_lval = make_data_lval(wasm, lval);
  return inter_data_lval(wasm, data_lval);
}

CResult inter_list(Wasm* wasm, Lval* lval) {
  Cell* head = lval->head;
  int count = list_count(head);
  int* data_cells[count];
  int i = 0;
  /* printf("inter list\n"); */
  while (head) {
    int v_ptr = datafy_lval(wasm, head->car).wasm_ptr;
    /* lval_println(head->car); */
    int* data_cell = make_data_cell(wasm, head);
    data_cell[car_offset] = v_ptr;
    data_cells[i++] = data_cell;
    head = head->cdr;
  }
  int cdr = 0;
  while (i--) {
    int* data_cell = data_cells[i];
    data_cell[cdr_offset] = cdr;
    cdr = inter_data_cell(wasm, data_cell).wasm_ptr;
  }
  int* data_list = make_data_lval(wasm, lval);
  data_list[head_offset] = cdr;
  return inter_data_lval(wasm, data_list);
}

#define wval_fun_type_size 5 * 4
#define wval_fun_size slot_type_size + wval_fun_type_size

#define wval_type_offset 0
#define subtype_offset 1

#define fn_table_index_offset 2
#define param_count_offset 4
#define has_rest_arg_offset 6
#define partial_count_offset 8
#define closure_offset 12
#define partials_offset 16
/* #define str_offset 4  // 20 */

int* make_data_lval_wasm_lambda(Wasm* wasm, int fn_table_index, int param_count,
                                int has_rest_arg) {
  int* data_lval = calloc(1, wval_fun_size);
  long p = (long)data_lval;
  /* int string_offset = 0; */
  /* if (lval->str) string_offset = add_string_to_data(wasm, lval->str); */

  *(int*)(p + ref_count_offset * 4) = 1;
  *(int*)(p + data_p_offset * 4) =
      wasm->__data_end + wasm->data_offset + slot_type_size;
  p += slot_type_size;

  *(char*)(p + wval_type_offset) = LVAL_WASM_LAMBDA;
  *(char*)(p + subtype_offset) = -1;
  *(short*)(p + fn_table_index_offset) = fn_table_index;
  *(short*)(p + param_count_offset) = param_count;
  *(short*)(p + has_rest_arg_offset) = has_rest_arg;
  *(short*)(p + partial_count_offset) = 0;
  *(int*)(p + closure_offset) = 0;
  *(int*)(p + partials_offset) = 0;

  return (int*)data_lval;
}

CResult inter_data_lval_wasm_lambda(Wasm* wasm, int* data_lval) {
  int offset = add_bytes_to_data(wasm, (char*)data_lval, wval_fun_size);
  int wval_ptr = wasm->__data_end + offset + slot_type_size;
  printf("wval_ptr %d\n", wval_ptr);
  CResult ret = {.ber = make_int32(wasm->module, wval_ptr),
                 .wasm_ptr = wval_ptr};
  return ret;
}

CResult inter_lval_str_type(Wasm* wasm, Cell** pool, Lval* lval) {
  CResult* ret = (CResult*)alist_get(*pool, is_eq_str, lval->str);
  if (!ret) {
    ret = malloc(sizeof(CResult));
    *ret = inter_lval(wasm, lval);
    *pool = alist_prepend(*pool, lval->str, ret);
  }
  return *ret;
}
