#include "platform.h"

char* free_p;

export_wasm void init_malloc() {
  free_p = get_memory();
  printf("init_malloc, heap_base = %li\n", (long)free_p);
}

export_wasm void free_malloc() { free_memory(); }

export_wasm void* _malloc(int size) {
  char* next_free_p =
      free_p + (size + 4 - size % 4);  // align to dword (32 bits)
  /* printf("_malloc: mem start %li, allocate: %d, next_free_p %li, mem_end
   * %li\n", */
  /*        (long)get_memory(), size, (long)(next_free_p),
   * (long)(get_mem_end())); */
  while (next_free_p >= get_mem_end()) {
    if (!grow_memory()) {
      printf("Error: malloc failed\n");
      /* exit(1); */
    }
  }

  char* current_free_p = free_p;
  free_p = next_free_p;
  return current_free_p;
}

export_wasm void _free(void* p) {}

export_wasm void* _realloc(void* p, int old_size, int new_size) {
  void* new_p = _malloc(new_size);
  char* to_p = new_p;
  while (old_size--) *to_p++ = (*(char*)p++);
  return new_p;
}

export_wasm int test() {
  /* printf("%d\n", sizeof(short)); */
  /* printf("%d\n", sizeof(int)); */
  /* printf("Hello %b\n", 255); */
  /* printf("from mem: %d %d %d %d %d\n", mem[0], mem[1], mem[2], mem[3],
   * mem[4]); */
  /* printf("from mem[0]: %d \n", get_byte_at((void*)0)); */
  /* set_dword_at((void*)0,100000); */
  /* printf("from mem[1]: %d \n", get_byte_at((void*)0)); */
  /* printf("from mem[0]: %d \n", get_word_at((void*)0)); */
  /* printf("from mem[0]: %d \n", get_dword_at((void*)0)); */
  /* printf("__heap_base %u\n", __heap_base); */

  /* char* mem_start = get_memory(); */
  /* *(short*)mem_start = 65537; */
  /* printf("mem[%d] = %d\n", mem_start, *(short*)mem_start); */

  /* Mempool* mp = create_mempool(32, 10, 1, &printf); */
  /* int* slot1 = mempool_alloc(mp); */
  /* int* slot2 = mempool_alloc(mp); */
  /* *slot1 = 123; */
  /* *slot2 = 12345; */
  /* printf("slot1: %d\n", *slot1); */
  /* printf("slot2: %d\n", *slot2); */
  /* mempool_free(mp, slot1); */

  /* printf("slot1: %d\n", *slot1); */
  /* printf("slot2: %d\n", *slot2); */

  /* int* slot3 = mempool_alloc(mp); */
  /* printf("slot1: %d\n", *slot1); */
  /* printf("slot2: %d\n", *slot2); */
  /* printf("slot3: %d\n", *slot3); */
  /* *slot3 = 456; */

  /* printf("slot1: %d\n", *slot1); */
  /* printf("slot2: %d\n", *slot2); */
  /* printf("slot3: %d\n", *slot3); */
  return 42;
}
