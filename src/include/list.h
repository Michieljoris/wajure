#ifndef __LIST_H_
#define __LIST_H_

typedef struct cell Cell;

struct cell {
  void* car;
  Cell* cdr;
};

Cell* list_new(void);
Cell* list_copy(Cell* cell, Cell* till_cell);
void list_free(Cell* cell);
void* list_get(Cell* list, int cmp(void*));
void* list_nth(Cell* list, int n);
void list_print(Cell* cell, void print(Cell*), char seperator);
Cell* copy_cells(Cell* from_cell, Cell* to_cell, Cell* till_cell);
Cell* list_cons(Cell* cons, Cell* list);
void* list_first(Cell* list);
Cell* list_rest(Cell* list);
Cell* list_concat(Cell* list1, Cell* list2);
Cell* alist_last(Cell* list);

// Association list
void* alist_get(Cell* alist, int key_cmp(void*));
int key_is_bound(Cell* alist, int key_cmp(void*));
Cell* find_pair(Cell* alist, int key_cmp(void*));
// Mutable

void alist_assoc(Cell* alist, int key_cmp(void*), void* key, void* value);

// Persistent association list
Cell* alist_prepend(Cell* alist, void* key, void* value);
Cell* alist_passoc(Cell* alist, int key_cmp(void*), void* key, void* value);
#endif  // __LIST_H_
