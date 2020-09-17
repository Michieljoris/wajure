#include "platform.h"

#ifdef WASM

#include "printf.h"

extern char __heap_base;
char* heap_pointer = &__heap_base;

char* get_memory() { return heap_pointer; }

void free_memory() { /* No implementation possible in wasm */
}

#else  // not WASM
/*  */
// IO
void _putchar(char character) {
  // send char to console etc.
  putchar(character);
}

// MEMORY

// Simulating wasm
const int page_size = 64 * 1024;
const int max_page_count = 1000;  // 64 MB
int page_count = 1;
const long int max_size = page_size * max_page_count;

char* memory;

char* get_memory() {
  if (!memory) memory = calloc(1, max_size);
  return memory;
}

char* get_mem_end() { return memory + page_count * page_size; }

void free_memory() { free(memory); }

int grow_memory() {
  /* printf("Grow_memory\n"); */
  if (++page_count > max_page_count) {
    printf("Error: can't allocate memory beyond max\n");
    return 0;
  }
  return 1;
}

// Gets the content of what's at address p, expected to return a pointer
/* void* get_pointer_at(void** p) { return *p; } */

// Sets the content of address p1 to p2
/* void set_pointer_at(void** p1, void* p2) { *p1 = p2; } */

// Puts value that's at from_p into address at to_p
/* void copy_byte(const char* from_p, char* to_p) { *to_p = *from_p; } */

#endif  // WASM

/* page_count = (next_free_p - memory) / page_size + 1; */
