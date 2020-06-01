/* http://www.pinksquirrellabs.com/blog/2018/01/31/-fixed-memory-pool-design/ */

#include "memory_pool.h"

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;

/*
 * Resizes pool when full. Since pool might be moved by realloc returned
 * pointers from alloc are relative.
 */

MemoryPool* create_memory_pool(  // MemoryPool** memory_pool_address,
    int element_size, uint initial_element_count) {
  /* Allocate memory for MemoryPool struct plus data (count * size) */
  int dataBlockSize = (element_size * initial_element_count);
  MemoryPool* memory_pool =
      (MemoryPool*)malloc(sizeof(MemoryPool) + dataBlockSize);
  memory_pool->element_count = initial_element_count;
  memory_pool->element_size = element_size;

  /* Set free_offset to beginning of allocated (data) block  */
  memory_pool->free_offset = 0;
  /* memory_pool->memory_pool_address = memory_pool_address; */
  memory_pool->memory_pool_address = &memory_pool;

  // Initialize data with pointers creating a linked list
  long int address =
      (long int)&memory_pool->data;  // address points to data block
  int offset = 0;
  for (int i = 0; i < initial_element_count - 1; i++) {
    offset += element_size;
    *(long int*)address = offset;
    address += element_size;
  }

  // Last block's pointer is -1, meaning, no more blocks available.
  *(int*)address = -1;

  // assign memory_pool_addres to the new address
  /* *memory_pool_address = memory_pool; */

  return memory_pool;
}

void destroy_pool(MemoryPool* mp) { free(mp); }

int memory_pool_alloc(MemoryPool* memory_pool) {
  int offset = memory_pool->free_offset;

  // If we have run out of space let's resize the pool
  if (offset == -1) {
    int oldCount = memory_pool->element_count;
    int newCount = oldCount * 2;
    memory_pool->element_count = newCount;
    int dataBlockSize = (newCount * memory_pool->element_size);
    int newSize = dataBlockSize + sizeof(MemoryPool);
    printf("out of memory, reallocating as size %i\n", newSize);
    memory_pool = (MemoryPool*)realloc((void*)memory_pool, newSize);

    // Point offset at the first element of extra data block
    offset = memory_pool->free_offset =
        (oldCount + 1) * memory_pool->element_size;
    long int address = (long int)&memory_pool->data + offset;
    // Initialize new data block with pointers creating a linked list
    for (int i = oldCount; i < newCount - 1; i++) {
      offset += memory_pool->element_size;
      *(int*)address = offset;
      address += memory_pool->element_size;
    }
    *(int*)address = -1;
    offset = memory_pool->free_offset;

    // rewrite memory_pool_addres's reference address
    *memory_pool->memory_pool_address = memory_pool;
  }

  // Pointer to free element
  int* element = (int*)((long int)&memory_pool->data + offset);
  // Set free_offset to whatever is at *element
  memory_pool->free_offset = *element;
  // Might be -1 now.
  return offset;
}

void memory_pool_free(MemoryPool* memory_pool, int offset) {
  /* Pointer to element to free up*/
  int* element = (int*)((long int)&memory_pool->data + offset);
  /* Set where element is pointing to the current free_offset */
  *element = memory_pool->free_offset;
  /* Point free_offset to this element, which is now free */
  memory_pool->free_offset = offset;
}

// Pool might have been moved since initialising, so to get address of block
// this fn will have to be used if there's any chance of an allocation having
// happened in the meantime.
void* memory_pool_get(MemoryPool* memory_pool, int offset) {
  // Calculate address of element at offset in pool
  int* address = (int*)((long int)&memory_pool->data + offset);
  return (void*)address;
}

/* void foo() { */
/*   // This pointer to a pool gets set by init and updated by alloc when
 * needed. */
/*   /\* MemoryPool* mp = malloc(sizeof(MemoryPool*)); *\/ */
/*   /\* memory_pool_init(&mp, 10, 1); *\/ */
/*   MemoryPool* mp = create_memory_pool(10, 1); */
/* } */
