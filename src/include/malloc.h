#ifndef __MALLOC_H_
#define __MALLOC_H_

void init_malloc(int initial_page_count, int _max_page_count, int _page_size);

void free_malloc();

void* _malloc(int size);

void* _realloc(void* p, int old_size, int new_size);

#endif  // __MALLOC_H_
