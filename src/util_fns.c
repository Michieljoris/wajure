#include "util_fns.h"

#include "assert.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "platform.h"
#include "print.h"
#include "read.h"

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
  return make_lval_nil();
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

Lval* read_string_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("read-string", 1)
  ITER_NEXT_TYPE(LVAL_LITERAL, STRING)
  int pos = 0;
  return read_expr(arg->str, &pos, '\0');
}

Lval* str_fn(Lenv* env, Lval* arg_list) {
  scoped_iter Cell* i = iter_new(arg_list);
  Lval* arg = iter_next(i);
  int maxlen = MAX_CHAR_SIZE;
  scoped char* str = lalloc_size(maxlen);
  char* ptr = str;
  while (arg) {
    int len = lval_snprint(ptr, maxlen, arg);
    int real_len = _strlen(ptr);
    if (len > real_len) {
      lval_println(arg_list);
      return make_lval_err("String can only hold a max of %d chars",
                           MAX_CHAR_SIZE);
    }
    maxlen -= len;
    ptr += len;
    arg = iter_next(i);
  }
  str = lrealloc(str, _strlen(str) + 1);
  return make_lval_str(str);
}

Lval* partial_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW("partial")
  ITER_NEXT_TYPE(LVAL_FUNCTION, STRING)
  int pos = 0;
  return read_expr(arg->str, &pos, '\0');
}

LispyFn util_builtin_fns[] = {
    {"print", print_fn, "print_fn", 2, 1},
    {"pr", pr_fn, "pr_fn", 2, 1},
    {"debug", debug_fn, "debug_fn", 2, 1},
    {"boolean", boolean_fn, "boolean_fn", 2, 1},
    {"hash", hash_fn, "hash_fn", 2, 1},
    {"str", str_fn, "str_fn", 2, 1},
    {"read-string", read_string_fn, "read_string_fn", 2, 1},
    {"partial", partial_fn, "partial_fn", 2, 1},
    {NIL}

};
