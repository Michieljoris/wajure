#include "assert.h"
#include "env.h"
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
  return lval_list2;
}

Lval* first_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("first", 1)
  ITER_NEXT_TYPE(LVAL_COLLECTION, -1)
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

  Lval* lval_list = NULL;
  lval_list = make_lval_list();
  lval_list->head = list_concat(lval_list1->head, lval_list2->head);
  return lval_list;
}

Lval* count_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("count", 1)
  ITER_NEXT_TYPE(LVAL_COLLECTION, -1)
  int count = list_count(arg->head);
  ITER_END
  return make_lval_num(count);
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

Builtin list_builtins[8] = {

    {"cons", cons_fn},
    {"first", first_fn},
    {"list", list_fn},
    {"rest", rest_fn},
    {"concat", concat_fn},
    {"count", count_fn},
    {"nth", nth_fn},
    {NULL}

};
