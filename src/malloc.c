#include "platform.h"

char* free_p;

void init_malloc() { free_p = get_memory(); }

void free_malloc() {}

void* _malloc(int size) {
  char* next_free_p = free_p + size;
  while (next_free_p >= get_mem_end()) {
    if (!grow_memory()) {
      printf("Error: malloc failed\n");
      exit(1);
    }
  }

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
