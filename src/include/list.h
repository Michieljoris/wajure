#ifndef __LIST_H_
#define __LIST_H_

#include "ltypes.h"

Cell* list_new(void);
Cell* list_get(Cell* list, int cmp(void*, void*), void* key);

int list_count(Cell* list);
void* list_nth(Cell* list, int n);
void list_print(Cell* cell, void print(void*), char* seperator);
Cell* copy_cells(Cell* from_cell, Cell* to_cell, Cell* till_cell);
Cell* list_cons(void* cons, Cell* list);
void* list_first(Cell* list);
Cell* list_rest(Cell* list);
Cell* list_concat(Cell* list1, Cell* list2);
Cell* list_last(Cell* list);

Cell* make_pair(void* key, void* value);

// Association list

void* alist_get(Cell* alist, int key_cmp(void*, void*), void* key);
int alist_has_key(Cell* alist, int key_cmp(void*, void*), void* key);

// Mutable
Cell* alist_put(Cell* alist, int key_cmp(void*, void*), void* key, void* value);

// Persistent association list
Cell* alist_prepend(Cell* alist, void* key, void* value);
/* Cell* alist_passoc(Cell* alist, int key_cmp(void*, void*), void* key, */
/*                    void* value); */
#endif  // __LIST_H_
