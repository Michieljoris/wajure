#include "io.h"
#include "lval_mempool.h"

Cell* make_cell() {
  Cell* cell = mempool_alloc(lval_mempool);
  cell->car = NULL;
  cell->cdr = NULL;
  return cell;
}

Cell* copy_cells(Cell* from_cell, Cell* to_cell, Cell* till_cell) {
  Cell* prev_cell = to_cell;
  to_cell->car = from_cell->car;
  from_cell = from_cell->cdr;
  while (from_cell != till_cell) {
    to_cell = make_cell();
    to_cell->car = from_cell->car;
    prev_cell->cdr = to_cell;
    prev_cell = to_cell;
    from_cell = from_cell->cdr;
  }
  return to_cell;
}

Cell* list_new() { return make_cell(); }

Cell* list_copy(Cell* cell, Cell* till_cell) {
  if (!cell) return cell;
  Cell* new_head = make_cell();
  copy_cells(cell, new_head, till_cell);
  return new_head;
}

void list_free(Cell* cell) {
  // Ref counting should get rid of rest of list
  mempool_free(lval_mempool, cell);
}

void* list_get(Cell* list, int cmp(void*)) {
  while (list) {
    if (cmp(list->car)) {
      return list->car;
    }
    list = list->cdr;
  }
  return NULL;
}

void* list_nth(Cell* list, int n) {
  Cell* ret = NULL;
  int counter = 0;
  while (list) {
    if (counter++ == n) {
      return list->car;
    }
    list = list->cdr;
  }
  return ret;
}

Cell* alist_last(Cell* list) {
  while (list) {
    if (!list->cdr) return list;
    list = list->cdr;
  }
  return list;
}

Cell* list_cons(Cell* cons, Cell* list) {
  Cell* new_head = make_cell();
  new_head->car = cons;
  new_head->cdr = list;
  return new_head;
}
void* list_first(Cell* list) { return list->car; }
Cell* list_rest(Cell* list) { return list->cdr; }
Cell* list_concat(Cell* list1, Cell* list2) {
  if (!list1) return list2;
  Cell* tail = alist_last(list1);
  tail->cdr = list2;
  return list1;
}

void list_print(Cell* cell, void print(Cell*), char seperator) {
  while (cell) {
    print(cell->car);
    cell = cell->cdr;
    if (cell) putchar(seperator);
  }
}

// Association list

void* alist_get(Cell* alist, int key_cmp(void*)) {
  while (alist) {
    Cell* pair = (Cell*)alist->car;
    if (key_cmp(pair->car)) {
      return pair->cdr;
    }
    alist = alist->cdr;
  }
  return NULL;
}

int key_is_bound(Cell* alist, int key_cmp(void*)) {
  return alist_get(alist, key_cmp) ? 1 : 0;
}

Cell* find_pair(Cell* alist, int key_cmp(void*)) {
  while (alist) {
    Cell* pair = (Cell*)alist->car;
    if (key_cmp(pair->car)) return pair;
  }
  return NULL;
}

// Mutable
void alist_assoc(Cell* alist, int key_cmp(void*), void* key, void* value) {
  Cell* pair = find_pair(alist, key_cmp);
  if (pair) {
    pair->car = value;
  } else {
    Cell* tail = alist_last(alist);
    pair = make_cell();
    tail->cdr = pair;
    pair->car = key;
    pair->cdr = value;
  }
}

// Persistent association list

Cell* alist_pcopy(Cell* palist) { return palist; }

// pput: just stick it in front of the lookup list
Cell* alist_prepend(Cell* alist, void* key, void* value) {
  Cell* pair = make_cell();
  pair->car = key;
  pair->cdr = value;
  return list_cons(pair, alist);
}

// passoc: copy path to cell
Cell* alist_passoc(Cell* alist, int key_cmp(void*), void* key, void* value) {
  Cell* pair = find_pair(alist, key_cmp);
  if (pair) {
    Cell* new_head = make_cell();
    Cell* tail = copy_cells(alist, new_head, pair);
    Cell* pair = make_cell();
    pair->car = key;
    pair->cdr = value;
    Cell* new_cell = make_cell();
    tail->cdr = new_cell;
    new_cell->car = pair;
    new_cell->cdr = tail->cdr->cdr;
    return new_head;
  } else {
    return alist_prepend(alist, key, value);
  }
}
