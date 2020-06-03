#include "io.h"
#include "list.h"

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
