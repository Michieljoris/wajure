#include "assert.h"
#include "cell.h"
#include "env.h"
/* #include "hash.h" */
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "print.h"

Lval* cons_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("cons", 2)
  ITER_NEXT
  Lval* x = arg;
  ITER_NEXT_TYPE(LVAL_COLLECTION, -1)
  Lval* lval_list = arg;
  ITER_END
  Lval* lval_list2 = make_lval_list();
  lval_list2->head = list_cons(x, lval_list->head);
  /* lval_list2->hash = lval_list2->head->hash = */
  /*     calc_list_hash(lval_list2->head, lval_list->head); */
  return lval_list2;
}

Lval* first_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("first", 1)
  /* ITER_NEXT_TYPE(LVAL_COLLECTION, -1) */
  ITER_NEXT
  if (arg->subtype == LNIL) return retain(arg);
  LASSERT_TYPE("first", arg_list, 1, LVAL_COLLECTION, -1, arg)
  Lval* lval_list = arg;
  ITER_END
  Lval* lval = list_first(lval_list->head);
  ITER_END
  return lval ? lval : make_lval_nil();
}

Lval* rest_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("rest", 1)
  ITER_NEXT_TYPE(LVAL_COLLECTION, -1)
  Lval* lval_list = arg;
  ITER_END
  Lval* lval_list2 = make_lval_list();
  lval_list2->head = list_rest(lval_list->head);
  /* printf("lval_list2->head->hash %d \n", lval_list->head->hash); */
  /* if (lval_list2->head) lval_list2->hash = lval_list2->head->hash; */
  return lval_list2;
}

Lval* list_fn(Lenv* env, Lval* arg_list) {
  scoped_iter Cell* i = iter_new(arg_list);
  Lval* arg = iter_next(i);
  Lval* lval_list = make_lval_list();
  Cell** lp = &(lval_list->head);
  while (arg) {
    Cell* next_cell = make_cell();
    next_cell->car = arg;
    retain(arg);
    *lp = next_cell;
    lp = &(next_cell->cdr);
    arg = iter_next(i);
  }
  return lval_list;
}

Lval* concat_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("concat", 2)
  ITER_NEXT_TYPE(LVAL_COLLECTION, LIST)
  Lval* lval_list1 = arg;
  ITER_NEXT_TYPE(LVAL_COLLECTION, LIST)
  Lval* lval_list2 = arg;
  ITER_END

  Lval* lval_list = NIL;
  lval_list = make_lval_list();
  lval_list->head = list_concat(lval_list1->head, lval_list2->head);
  return lval_list;
}

Lval* foo_fn(Lenv* env, Lval* arg_list) {
  printf("in foo fn arg_list: %li\n", (long)arg_list);
  Cell* head = arg_list->head;
  printf("head: %li\n", (long)head);
  while (head) {
    printf("head->car: %li\n", (long)head->car);
    printf("head->cdr: %li\n", (long)head->cdr);
    Lval* arg = (Lval*)head->car;
    printf("type of arg: %s\n", lval_type_constant_to_name(arg->type));
    head = head->cdr;

    Cell* h = arg->head;
    while (h) {
      printf("h->car: %li\n", (long)h->car);
      printf("h->cdr: %li\n", (long)h->cdr);
      h = h->cdr;
    }
  }
  /* lval_println((Lval*)arg_list->head->car); */
  /* printf("in foo fn nil: %li\n", (long)make_lval_nil()); */
  return make_lval_nil();
}

Lval* count_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("count", 1)
  ITER_NEXT_TYPE(LVAL_COLLECTION, -1)
  int count = list_count(arg->head);
  ITER_END
  return make_lval_num(count);
}

Lval* is_list_fn(Lenv* env, Lval* arg_list) {
  Lval* result;
  ITER_NEW_N("list?", 1)
  ITER_NEXT
  if (arg->subtype == LIST)
    result = make_lval_true();
  else
    result = make_lval_false();
  ITER_END
  return result;
}

Lval* nth_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("nth", 2)
  ITER_NEXT_TYPE(LVAL_COLLECTION, -1)
  Lval* coll = arg;
  ITER_NEXT_TYPE(LVAL_LITERAL, NUMBER)
  long int index = arg->num;
  Lval* nth_lval = list_nth(coll->head, arg->num);
  ITER_END
  if (!nth_lval) {
    return make_lval_err(
        "Index out of bounds for nth function for index %li, length of "
        "collection is %d",
        index, list_count(coll->head));
  }
  return nth_lval;
}

LispyFn list_builtin_fns[] = {

    {"cons", cons_fn, "cons_fn", 2, 1},
    {"first", first_fn, "first_fn", 2, 1},
    {"list", list_fn, "list_fn", 2, 1},
    {"rest", rest_fn, "rest_fn", 2, 1},
    {"concat", concat_fn, "concat_fn", 2, 1},
    {"count", count_fn, "count_fn", 2, 1},
    {"nth", nth_fn, "nth_fn", 2, 1},
    {"list?", is_list_fn, "is_list_fn", 2, 1},
    {"foo", foo_fn, "foo_fn", 2, 1},
    {NIL}

};
