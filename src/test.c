#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "lispy_mempool.h"
#include "list.h"
#include "plist_fns.h"

void test_mempool() {
  /* Mempool* mp = create_mempool(4, 2, MEMPOOL_AUTO_RESIZE); */

  /* int* d1 = mempool_alloc(mp); */
  /* *d1 = 111; */
  /* printf("%d\n", *d1); */

  /* int* d2 = mempool_alloc(mp); */
  /* *d2 = 222; */
  /* printf("%d\n", *d2); */

  /* mempool_debug(mp); */
  /* /\* memory_pool_free(mp, d2); *\/ */

  /* /\* memory_pool_debug(mp); *\/ */

  /* int* d3 = mempool_alloc(mp); */
  /* *d3 = 333; */
  /* printf("%d\n", *d3); */

  /* printf("Done"); */
}

void test_plist() {
  printf("Test plist\n");

  init_lispy_mempools(1, 1, 100);
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

void print_char_cell(void* value) { printf("%s", (char*)value); }
void print_pair(void* pair) {
  printf("%s: %s", (char*)((Cell*)pair)->car, (char*)((Cell*)pair)->cdr);
}
int cmp(void* k, void* v) { return strcmp((char*)k, (char*)v) == 0; }

void test_list() {
  init_lispy_mempools(1, 1, 100);
  printf("Testing list\n");
  Cell* list = list_new();
  list->car = "1";
  list = list_cons("2", list);
  list = list_cons("3", list);
  list_print(list, print_char_cell, ",");
  Cell* list2 = list_copy(list, NULL);
  list = list_cons("4", list);
  list_print(list, print_char_cell, ",");
  list_print(list2, print_char_cell, ",");
  Cell* cell = list_get(list, "1", cmp);
  printf("%s\n", (char*)cell->car);
  printf("%s\n", (char*)cell->car);
  printf("%s\n", (char*)list_nth(list2, 0));
  printf("%s\n", (char*)list_last(list2)->car);
  printf("count: %d\n", list_count(list));
  printf("count: %d\n", list_count(list2));
  printf("%s\n", (char*)list_first(list));
  list_print(list_rest(list2), print_char_cell, ",");
  list_print(list_concat(list, list2), print_char_cell, ",");
  printf("done\n");
  Cell* alist = list_new();

  alist_assoc(alist, cmp, "foo", "bar");
  alist_assoc(alist, cmp, "fox", "foz");
  alist_assoc(alist, cmp, "2a", "3");
  /* list_print(alist, print_pair, "\n"); */
  alist_assoc(alist, cmp, "fox", "foz2");
  list_print(alist, print_pair, "\n");
  alist = alist_prepend(alist, "the", "end");
  list_print(alist, print_pair, "\n");
  printf("palist-------------------------:\n");
  Cell* alist2 = alist_passoc(alist, cmp, "one", "more");
  list_print(alist, print_pair, "\n");

  printf("palist-------------------------:\n");
  list_print(alist2, print_pair, "\n");

  list_print(alist, print_pair, ", ");
  Cell* alist3 = alist_passoc(alist, cmp, "foo", "not bar");

  printf("alist-------------------------:\n");
  list_print(alist, print_pair, ", ");
  printf("alist3-------------------------:\n");
  list_print(alist3, print_pair, ", ");

  printf("printing empty list\n");
  list_print(list_new(), print_pair, "\n");
  printf("done again\n");
  printf("%s\n", (char*)alist_get(alist3, cmp, "foo"));
  printf("%d\n", alist_has_key(alist3, cmp, "foo"));
}
