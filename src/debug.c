#include "debug.h"

#include <stdlib.h>

#include "lval_mempool.h"
#include "plist_fns.h"

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
  init_lval_mempool(100);
  /* Lval* plist = make_lval_plist(); */
  /* printf("cdr: %p\n", plist->cdr); */
  /* Lval* lval_num = make_lval_num(123); */
  /* printf("cdr: %p\n", lval_num->cdr); */
  /* Lval* result_plist = cons_fn(lval_num, plist); */
  /* result_plist = cons_fn(lval_num, result_plist); */
  /* printf("plist cdr: %p\n", plist->cdr); */
  /* printf("num cdr: %p\n", lval_num->cdr); */
  /* lval_println(plist); */
  /* lval_println(result_plist); */
}
