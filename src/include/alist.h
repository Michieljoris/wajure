#ifndef __ALIST_H_
#define __ALIST_H_

#include "list.h"

Cell* alist_get(Cell* alist, int cmp(Cell*));

Cell* alist_nth(Cell* alist, int i);

int key_is_bound(Cell* alist, int cmp(Cell*));

Cell* find_pair(Cell* alist, int key_cmp(void*));

// Mutable
Cell* alist_assoc(Cell* alist, int cmp(Cell*), void free(void*), void* key,
                  void* value);

#endif  // __ALIST_H_
