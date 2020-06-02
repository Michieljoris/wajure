#include "debug.h"

#include <stdlib.h>

#include "mempool.h"
#include "plist.h"

int debug_level = 1;
int* pdebug_level = &debug_level;

void set_debug_level(int level) {
  /* debug->level = level; */
  *pdebug_level = level;
}

void test_memory_pool() {
  Mempool* mp = create_mempool(4, 2, MEMPOOL_AUTO_RESIZE);

  int* d1 = mempool_alloc(mp);
  *d1 = 111;
  printf("%d\n", *d1);

  int* d2 = mempool_alloc(mp);
  *d2 = 222;
  printf("%d\n", *d2);

  mempool_debug(mp);
  /* memory_pool_free(mp, d2); */

  /* memory_pool_debug(mp); */

  int* d3 = mempool_alloc(mp);
  *d3 = 333;
  printf("%d\n", *d3);

  printf("Done");
}

void test_plist() {
  printf("Test plist\n");
  Mempool* mempool = create_mempool(sizeof(Lval), 100, MEMPOOL_AUTO_RESIZE);
  Lval* plist = make_lval_plist(mempool);
  lval_println(plist);
}
