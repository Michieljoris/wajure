#include <io.h>

#include "alist.h"
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
