#ifndef __LVAL_MEMPOOL_H_
#define __LVAL_MEMPOOL_H_

#include "mempool.h"
#include "refcount.h"

// The order and number of these types needs to match!!
enum {
  LVAL,
  LENV,
  CELL,
  ITER,
  INTERNAL,
  CHAR8,
  CHAR16,
  CHAR32,
  CHAR64,
  CHAR128,
  CHAR256,
  CHAR512,
  SLOT_TYPE_COUNT
};

#define MAX_CHAR_SIZE 512

void init_lispy_mempools(uint lval_count, int lenv_count, int cell_count);
void free_lispy_mempools();

void* lalloc_type(int type);
void* lalloc_size(int size);
void* lrealloc(void* data_p, int size);

void print_mempool_free(int type);
void print_mempool_counts();

typedef struct {
  int env;
  int lval;
  int cell;
  int iter;
} SlotCount;

SlotCount get_slot_count();
#endif  // __LVAL_MEMPOOL_H_
