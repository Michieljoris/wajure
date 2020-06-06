#ifndef __LVAL_MEMPOOL_H_
#define __LVAL_MEMPOOL_H_

#include "mempool.h"

enum { LVAL, LENV, CELL };
char* slot_types[3];

Mempool* lval_mempool;
void init_lispy_mempools(uint lval_count, int lenv_count, int cell_count);
void free_lispy_mempools();

void* lalloc(int type);
void lfree(int type, void* slot);

#endif  // __LVAL_MEMPOOL_H_
