#include "lispy_mempool.h"

#include <stdlib.h>

#include "io.h"
#include "lval.h"
#include "mempool.h"

Mempool** mempools;

void init_lispy_mempools(uint lval_count, int lenv_count, int cell_count) {
  mempools = malloc(sizeof(Mempool*) * 3);
  mempools[LVAL] =
      create_mempool(sizeof(Lval), lval_count, MEMPOOL_AUTO_RESIZE);
  mempools[LENV] =
      create_mempool(sizeof(Lenv), lenv_count, MEMPOOL_AUTO_RESIZE);
  mempools[CELL] =
      create_mempool(sizeof(Cell), cell_count, MEMPOOL_AUTO_RESIZE);
}

void free_lispy_mempools() {
  free_mempool(mempools[LVAL]);
  free_mempool(mempools[LENV]);
  free_mempool(mempools[CELL]);
  free(mempools);
}

void* lalloc(int type) { return mempool_alloc(mempools[type]); }
void lfree(int type, void* slot) { return mempool_free(mempools[type], slot); }
