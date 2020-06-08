#include "assert.h"
#include "env.h"
#include "io.h"
#include "iter.h"
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
  lval_del(arg_list);
  return lval_list2;
}

Lval* first_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("first", 1)
  ITER_NEXT_TYPE(LVAL_COLLECTION, LIST)
  Lval* lval_list = arg;
  ITER_END
  Lval* lval = list_first(lval_list->head);
  lval_del(arg_list);
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
  lval_del(arg_list);
  return lval_list2;
}

Lval* list_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW("list")
  ITER_NEXT
  Lval* lval_list = make_lval_list();
  Cell** lp = &(lval_list->head);
  while (arg) {
    Cell* next_cell = make_cell();
    next_cell->car = arg;
    *lp = next_cell;
    lp = &(next_cell->cdr);
    ITER_NEXT
  }
  ITER_END
  lval_del(arg_list);
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
  if (!lval_list1->head)
    lval_list = lval_list2;
  else if (!lval_list2->head)
    lval_list = lval_list1;
  else {
    lval_list = make_lval_list();
    lval_list->head = list_concat(lval_list1->head, lval_list2->head);
  }
  lval_del(arg_list);
  return lval_list;
}

void lenv_add_list_fns(Lenv* env) {
  lenv_add_builtin(env, "cons", cons_fn, SYS);
  lenv_add_builtin(env, "first", first_fn, SYS);
  lenv_add_builtin(env, "list", list_fn, SYS);
  lenv_add_builtin(env, "rest", rest_fn, SYS);
  lenv_add_builtin(env, "concat", concat_fn, SYS);
}
