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
  ITER_NEXT_TYPE(LVAL_COLLECTION, LIST)
  Lval* lval_list = arg;
  ITER_END
  Lval* lval_list2 = make_lval_list();
  lval_list2->head = list_cons(x, lval_list->head);
  return lval_list2;
}

Lval* first_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("first", 1)
  ITER_NEXT_TYPE(LVAL_COLLECTION, LIST)
  Lval* lval_list = arg;
  ITER_END
  Lval* lval = list_first(lval_list->head);
  ITER_END
  return lval ? lval : make_lval_list();
}

Lval* rest_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("rest", 1)
  ITER_NEXT_TYPE(LVAL_COLLECTION, LIST)
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

Builtin list_builtins[6] = {

    {"cons", cons_fn},
    {"first", first_fn},
    {"list", list_fn},
    {"rest", rest_fn},
    {"concat", concat_fn},
    {NULL}

};
