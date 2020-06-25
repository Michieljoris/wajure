#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"

Cell* make_iter_cell() {
  Cell* cell = lalloc_type(ITER);
  cell->car = NIL;
  cell->cdr = NIL;
  return cell;
}

Cell* iter_new(Lval* lval_list) {
  Cell* iterator = make_iter_cell();
  iterator->car = lval_list->head;
  return iterator;
}

Cell* iter_cell(Cell* iterator) {
  if (!iterator->car) return NIL;
  return iterator->car;
}

Cell* iter_current_cell(Cell* iterator) {
  if (!iterator->cdr) return NIL;
  return iterator->cdr;
}

Lval* iter_next(Cell* iterator) {
  if (!iterator->car) {
    iterator->cdr = NIL;  // current cell
    return NIL;
  }
  Cell* p = iterator->car;
  Lval* next_lval = p->car;
  iterator->cdr = p;  // current cell
  iterator->car = p->cdr;
  return next_lval;
}

Lval* iter_peek(Cell* iterator) {
  if (!iterator->car) return NIL;
  Cell* p = iterator->car;
  Lval* next_lval = p->car;
  return next_lval;
}

void iter_end(Cell* iterator) {
  iterator->car = NIL;
  iterator->cdr = NIL;
  release(iterator);
}

void cleanup_iter(Cell** iterator) { iter_end(*iterator); }

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
