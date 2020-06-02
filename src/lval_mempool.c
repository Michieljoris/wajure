#include "lval_mempool.h"

#include "lval.h"
#include "mempool.h"

void init_lval_mempool(int size) {
  lval_mempool = create_mempool(sizeof(Lval), size, MEMPOOL_AUTO_RESIZE);
}
