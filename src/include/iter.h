#ifndef __ITER_H_
#define __ITER_H_
#include "assert.h"
#include "lval.h"

void cleanup_iter(Cell** iterator);
#define scoped_iter __attribute__((__cleanup__(cleanup_iter)))

Lval* next_arg(int do_expect, Cell* i, char* _fn_name, int _min_count,
               int _max_count, int _expected_count, int* _index,
               Lval* arg_list);

#define ITER_NEW_MIN_MAX(fn_name, min_count, max_count)          \
  char* _fn_name = fn_name;                                      \
  int _min_count = min_count;                                    \
  int _max_count = max_count;                                    \
  int _expected_count = min_count == max_count ? min_count : -1; \
  scoped_iter Cell* i = iter_new(arg_list);                      \
  int _index = 0;                                                \
  Lval* arg = NIL;

#define ITER_NEW(fn_name) ITER_NEW_MIN_MAX(fn_name, -1, 1);
#define ITER_NEW_N(fn_name, n) ITER_NEW_MIN_MAX(fn_name, n, n);
#define ITER_NEW_MIN(fn_name, min_count) \
  ITER_NEW_MIN_MAX(fn_name, min_count, -1);

#define ITER_NEXT                                                         \
  arg = next_arg(1, i, _fn_name, _min_count, _max_count, _expected_count, \
                 &_index, arg_list);                                      \
  if (arg && arg->group == LVAL_ERR) return arg;

#define ITER_NEXT_TYPE(expected_group, expected_type)                     \
  arg = next_arg(1, i, _fn_name, _min_count, _max_count, _expected_count, \
                 &_index, arg_list);                                      \
  if (arg->group == LVAL_ERR) return arg;                                 \
                                                                          \
  LASSERT_TYPE(_fn_name, arg_list, _index, expected_group, expected_type, arg);

#define ITER_END                                                          \
  arg = next_arg(0, i, _fn_name, _min_count, _max_count, _expected_count, \
                 &_index, arg_list);                                      \
  if (arg) return arg; /*error */

Cell* iter_new(Lval* lval_list);
Cell* iter_cell(Cell* iterator);
Cell* iter_current_cell(Cell* iterator);
Lval* iter_next(Cell* iterator);
void iter_end(Cell* iterator);
Lval* iter_peek(Cell* iterator);

#endif  // __ITER_H_
