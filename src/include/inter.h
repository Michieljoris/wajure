#ifndef __INTER_H_
#define __INTER_H_

#include "ltypes.h"
#include "wasm.h"

// Slot offsets
#define ref_count_offset 0
#define data_p_offset 12  // 4 bytes
#define slot_type_size 16

// Cell offsets
#define wcell_size slot_type_size + cell_type_size
#define cell_hash_offset slot_type_size + 0
#define car_offset slot_type_size + 4
#define cdr_offset slot_type_size + 8
#define cell_type_size cdr_offset + 4

// Lval offsets
#define type_offset slot_type_size + 0   // char
#define group_offset slot_type_size + 1  // char
#define d_offset slot_type_size + 4      // int
#define hash_offset slot_type_size + 8   // int

#define fn_table_index_offset slot_type_size + 12  // short
#define partial_count_offset slot_type_size + 14   // short
#define closure_offset slot_type_size + 16         // int
#define partials_offset slot_type_size + 20        // int
#define fn_call_relay_offset slot_type_size + 24   // int
#define lval_type_size fn_call_relay_offset + 4
/* #define str_offset 4  // 20 */

int inter_lval(Wasm* wasm, Lval* lval);
/* int inter_cell(Wasm* wasm, Cell* cell); */

int inter_list(Wasm* wasm, Lval* lval);

char* make_data_lval(Wasm* wasm, Lval* lval, int fn_table_index);
int inter_data_lval(Wasm* wasm, char* data_lval);

int inter_lval_str_type(Wasm* wasm, Cell** pool, Lval* lval);

void inter_rewrite_info(Wasm* wasm);

#endif  // __INTER_H_
