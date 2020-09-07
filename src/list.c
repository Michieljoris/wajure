#include "cell.h"
#include "io.h"
#include "lispy_mempool.h"
#include "ltypes.h"
#include "lval.h"
#include "print.h"

Cell* list_new() { return make_cell(); }

// Conses cons to list. Returns new head.
Cell* list_cons(void* cons, Cell* list) {
  Cell* new_head = make_cell();
  new_head->car = cons;
  new_head->cdr = list;
  retain(cons);
  retain(list);
  return new_head;
}
void* list_first(Cell* list) {
  if (list)
    return retain(list->car);
  else
    return list;
}

Cell* list_rest(Cell* list) {
  if (list)
    return retain(list->cdr);
  else
    return list;
}

// Copies from_cell list to to_cell list, until and excluding till_cell. Returns
// last cell in copied list
Cell* copy_cells(Cell* from_cell, Cell* to_cell, Cell* till_cell) {
  Cell* prev_cell = to_cell;
  to_cell->car = retain(from_cell->car);
  from_cell = from_cell->cdr;
  while (from_cell != till_cell) {
    to_cell = make_cell();
    to_cell->car = retain(from_cell->car);
    prev_cell->cdr = to_cell;
    prev_cell = to_cell;
    from_cell = from_cell->cdr;
  }
  return to_cell;
}

Cell* list_concat(Cell* list1, Cell* list2) {
  if (!list1) return retain(list2);
  if (!list2) return retain(list1);
  Cell* new_head = make_cell();
  Cell* tail = copy_cells(list1, new_head, NIL);
  tail->cdr = retain(list2);
  return new_head;
}

// memory leak safe ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Finds and returns the cell in list for which cmp returns true. cmp fn
// receives passed in key and each successive cell.
Cell* list_get(Cell* list, int cmp(void*, void*), void* key) {
  while (list) {
    if (cmp(key, list->car)) {
      return retain(list);
    }
    list = list->cdr;
  }
  return NULL;
}

// Returns the nth cell's car in list.
void* list_nth(Cell* list, int n) {
  Cell* ret = NULL;
  int counter = 0;
  while (list) {
    if (counter++ == n) {
      return retain(list->car);
    }
    list = list->cdr;
  }
  return ret;
}

// Returns the last cell in the list
Cell* list_last(Cell* list) {
  while (list) {
    if (!list->cdr) return retain(list);
    list = list->cdr;
  }
  return list;
}

// Returns number of cells in list
int list_count(Cell* list) {
  int i = 0;
  while (list) {
    i++;
    if (!list->cdr) break;
    list = list->cdr;
  }
  return i;
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

// Finds and returns cell for the appropriate association pair in alist for key.
// cmp fn receives passed in key and the key for each successive association.
Cell* find_cell(Cell* alist, int cmp_key(void*, void*), void* key) {
  if (!alist || !alist->car) return NULL;
  while (alist) {
    Cell* pair = (Cell*)alist->car;
    if (cmp_key(pair->car, key)) return alist;
    alist = alist->cdr;
  }
  return NULL;
}

// Finds and returns associated value in alist for key. cmp fn receives passed
// in key and the key for each successive association.
void* alist_get(Cell* alist, int cmp_key(void*, void*), void* key) {
  Cell* cell = find_cell(alist, cmp_key, key);
  if (cell) return ((Cell*)cell->car)->cdr;
  return NULL;
}

// Returns 1 if alist has key
int alist_has_key(Cell* alist, int cmp_key(void*, void*), void* key) {
  return find_cell(alist, cmp_key, key) ? 1 : 0;
}

// Conses kv value pair to alist. Returns new head.
Cell* alist_prepend(Cell* alist, void* key, void* value) {
  Cell* new_head = make_cell();
  new_head->cdr = alist;
  Cell* pair = make_cell();
  pair->car = key;
  pair->cdr = value;
  new_head->car = pair;
  return new_head;
}

// Mutates passed in alist. Replaces value if key is found, otherwise prepends
// alist with new association pair. Returns (possibly new) head. Retains
// key and value, and releases old value if updated.
Cell* alist_put(Cell* alist, int cmp_key(void*, void*), void* key,
                void* value) {
  Cell* node = find_cell(alist, cmp_key, key);
  if (node) {
    Cell* pair = ((Cell*)node->car);
    release(pair->cdr);
    pair->cdr = retain(value);
    return alist;
  } else {
    return alist_prepend(alist, retain(key), retain(value));
  }
}

// Persistent association list ========================================

// Immutable version of alist_assoc. Returns new alist. Both key and pair are
// assumed to be retained already.
/* Cell* alist_passoc(Cell* alist, int cmp_key(void*, void*), void* key, */
/*                    void* value) { */
/*   Cell* node = find_cell(alist, cmp_key, key); */
/*   if (node) { */
/*     Cell* new_head = make_cell(); */
/*     // tail is first cell after the cell we're 'updating'. */
/*     Cell* tail = copy_cells(alist, new_head, node); */
/*     // New cell with updated value: */
/*     Cell* pair = make_cell(); */
/*     pair->car = key; */
/*     pair->cdr = value; */
/*     // Use new cell to connect copied part of the list with shared part of
 * the */
/*     // list. */
/*     Cell* new_cell = make_cell(); */
/*     tail->cdr = new_cell; */
/*     new_cell->car = pair; */
/*     new_cell->cdr = retain(node); */
/*     return new_head; */
/*   } else { */
/*     return alist_prepend(alist, key, value); */
/*   } */
/* } */
