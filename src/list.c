#include "io.h"
#include "lispy_mempool.h"

Cell* make_cell() {
  Cell* cell = lalloc(CELL);
  cell->car = NULL;
  cell->cdr = NULL;
  return cell;
}

Cell* copy_cells(Cell* from_cell, Cell* to_cell, Cell* till_cell) {
  Cell* prev_cell = to_cell;
  to_cell->car = from_cell->car;
  from_cell = from_cell->cdr;
  while (from_cell != till_cell) {
    printf("copy_cells\n");
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
  lfree(CELL, cell);
}

Cell* list_get(Cell* list, void* key, int cmp(void*, void*)) {
  while (list) {
    if (cmp(key, list->car)) {
      return list;
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

Cell* list_last(Cell* list) {
  while (list) {
    if (!list->cdr) return list;
    list = list->cdr;
  }
  return list;
}

int list_count(Cell* list) {
  int i = 0;
  while (list) {
    i++;
    if (!list->cdr) break;
    list = list->cdr;
  }
  return i;
}

Cell* list_cons(void* cons, Cell* list) {
  Cell* new_head = make_cell();
  new_head->car = cons;
  new_head->cdr = list;
  return new_head;
}
void* list_first(Cell* list) { return list->car; }
Cell* list_rest(Cell* list) { return list->cdr; }
Cell* list_concat(Cell* list1, Cell* list2) {
  if (!list1) return list2;
  Cell* tail = list_last(list1);
  tail->cdr = list2;
  return list1;
}

void list_print(Cell* cell, void print(void*), char* seperator) {
  if (!cell || !cell->car) return;
  while (cell) {
    print(cell->car);
    cell = cell->cdr;
    if (cell) printf("%s", seperator);
  }
  printf("\n");
}

// Association list ========================================
Cell* find_node(Cell* alist, int cmp_key(void*, void*), void* key) {
  if (!alist || !alist->car) return NULL;
  while (alist) {
    Cell* pair = (Cell*)alist->car;
    if (cmp_key(pair->car, key)) return alist;
    alist = alist->cdr;
  }
  return NULL;
}

void* alist_get(Cell* alist, int cmp_key(void*, void*), void* key) {
  Cell* node = find_node(alist, cmp_key, key);
  if (node) return ((Cell*)node->car)->cdr;
  return NULL;
}

int alist_has_key(Cell* alist, int cmp_key(void*, void*), void* key) {
  return find_node(alist, cmp_key, key) ? 1 : 0;
}

// Mutable
void alist_assoc(Cell* alist, int cmp_key(void*, void*), void* key,
                 void* value) {
  Cell* node = find_node(alist, cmp_key, key);
  if (node) {
    ((Cell*)node->car)->cdr = value;
  } else {
    Cell* tail = list_last(alist);
    Cell* cell = NULL;
    if (!tail->car)
      cell = alist;
    else {
      cell = make_cell();
      tail->cdr = cell;
    }
    Cell* pair = make_cell();
    cell->car = pair;
    pair->car = key;
    pair->cdr = value;
  }
}

// Persistent association list ========================================
Cell* alist_pcopy(Cell* palist) { return palist; }

// prepend: just stick it in front of the lookup list
Cell* alist_prepend(Cell* alist, void* key, void* value) {
  Cell* pair = make_cell();
  pair->car = key;
  pair->cdr = value;
  return list_cons(pair, alist);
}

// passoc: copy path to cell
Cell* alist_passoc(Cell* alist, int cmp_key(void*, void*), void* key,
                   void* value) {
  Cell* node = find_node(alist, cmp_key, key);
  if (node) {
    Cell* new_head = make_cell();
    Cell* tail = copy_cells(alist, new_head, node);
    Cell* pair = make_cell();
    pair->car = key;
    pair->cdr = value;
    Cell* new_cell = make_cell();
    tail->cdr = new_cell;
    new_cell->car = pair;
    new_cell->cdr = node;
    return new_head;
  } else {
    return alist_prepend(alist, key, value);
  }
}
