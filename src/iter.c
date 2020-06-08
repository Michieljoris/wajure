#include "lib.h"
#include "lval.h"

Lval* next_arg(int do_expect, Cell* i, char* _fn_name, int _min_count,
               int _max_count, int _expected_count, int* _index,
               Lval* arg_list) {
  if (!i) return NIL;
  Lval* arg = iter_next(i);
  if (do_expect) {
    if (!arg) {
      int _count = list_count(arg_list->head);
      if (_expected_count >= 0 && *_index < _expected_count) {
        return make_lval_err(
            "function '%s' passed wrong number of args, "
            "got %i, expected %i",
            _fn_name, _count, _expected_count);
      }
      if (_min_count >= 0 && *_index < _min_count) {
        int _count = list_count(arg_list->head);
        return make_lval_err(
            "function '%s' passed wrong number of args, "
            "got %i, expected at least %i",
            _fn_name, _count, _min_count);
      }
    }
    (*_index)++;
    return arg;
  } else {
    if (arg) {
      if (_expected_count >= 0 && *_index >= _expected_count) {
        int _count = list_count(arg_list->head);
        return make_lval_err(
            "Function '%s' passed wrong number of args, "
            "got %i, expected %i",
            _fn_name, _count, _expected_count);
      }
      if (_max_count >= 0 && *_index >= _max_count) {
        int _count = list_count(arg_list->head);
        return make_lval_err(
            "Function '%s' passed wrong number of args, "
            "got %i, expected between %i and %i",
            _fn_name, _count, _min_count, _max_count);
      }
    }
    return NIL;
  }
}
