#include "malloc.h"

#include "lib.h"

#ifndef WASM
int page_size;
int max_page_count;
int page_count;
char* memory;
char* mem_end;

char* get_memory(int max_size) { return calloc(1, max_size); }

void grow_memory(char* next_free_p) {
  if (next_free_p >= memory + page_count * page_size)
    page_count = (next_free_p - memory) / page_size + 1;
}

void* get_pointer_at(void** p) { return *p; }

void set_pointer_at(void** p1, void* p2) { *p1 = p2; }

char* free_p;
void init_malloc(int initial_page_count, int _max_page_count, int _page_size) {
  page_size = _page_size;
  max_page_count = _max_page_count;
  int max_size = max_page_count * page_size;
  memory = get_memory(max_size);
  free_p = memory;
  mem_end = memory + max_size;
  page_count = initial_page_count;
}

void free_malloc() { free(memory); }
#endif

char* get_mem_end() { return mem_end; }

void* _malloc(int size) {
  char* next_free_p = free_p + size;
  if (next_free_p >= get_mem_end()) {
    printf("Error: can't allocate memory beyond max\n");
    exit(1);
  }
  grow_memory(next_free_p);

  char* current_free_p = free_p;
  free_p = next_free_p;
  return current_free_p;
}

void* _realloc(void* p, int old_size, int new_size) {
  void* new_p = _malloc(new_size);
  void* to_p = new_p;
  while (old_size--) copy_byte(p++, to_p++);
  return new_p;
}
