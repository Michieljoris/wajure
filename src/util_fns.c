#include "util_fns.h"

#include "assert.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "platform.h"
#include "print.h"

Lval* print_fn(Lenv* env, Lval* arg_list) {
  ddebug("executing print_fn:");
  scoped_iter Cell* i = iter_new(arg_list);
  Lval* arg = iter_next(i);
  while (arg) {
    lval_print(arg);
    putchar(' ');
    arg = iter_next(i);
  }
  putchar('\n');
  return make_lval_list();
}

Lval* pr_fn(Lenv* env, Lval* arg_list) {
  scoped_iter Cell* i = iter_new(arg_list);
  Lval* arg = iter_next(i);
  while (arg) {
    lval_pr(arg);
    putchar(' ');
    arg = iter_next(i);
  }
  putchar('\n');
  return make_lval_nil();
}

Lval* debug_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("debug", 1)
  ITER_NEXT_TYPE(LVAL_LITERAL, NUMBER)
  int num = arg->num;
  ITER_END
  ddebug("debug = %il\n", num);
  set_log_level((int)num);
  return make_lval_nil();
}

Lval* boolean_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("boolean", 1)
  ITER_NEXT
  if (arg->subtype == LNIL) return make_lval_false();
  if (arg->subtype == LFALSE) return retain(arg);
  ITER_END
  return make_lval_true();
}

Lval* hash_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("hash", 1)
  ITER_NEXT
  Lval* ret = make_lval_num(arg->hash);
  ITER_END
  /* printf("hash: %d\n", arg->hash); */
  return ret;
}

LispyFn util_builtin_fns[] = {{"print", print_fn, "print_fn", 2, 1},
                              {"pr", pr_fn, "pr_fn", 2, 1},
                              {"debug", debug_fn, "debug_fn", 2, 1},
                              {"boolean", boolean_fn, "boolean_fn", 2, 1},
                              {"hash", hash_fn, "hash_fn", 2, 1},
                              {NIL}

};
