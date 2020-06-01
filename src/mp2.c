#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;
typedef unsigned char uchar;

typedef struct memory_pool MemoryPool;

struct memory_pool {
  uint element_count;  // Num of blocks
  uint element_size;   // Size of each block

  uint free_block_count;  // Number of unallocated blocks
  uchar** free_block_p;   // Address of next free block

  void* data_p;             // Address of beginning of memory pool
  uchar** end_of_data_p;    // Address of first byte *after* data
  uchar** uninitialised_p;  // Pointer to remaining part of data that's not yet
                            // initialised

  /* uint numInitialized;  // Num of initialized blocks */
};

/*
 * Does not resize pool when full. Returns absolute pointers to allocated
 * blocks, which is ok since the pool is not reallocated at any time.
 */

MemoryPool* create_memory_pool(int element_size, uint element_count) {
  MemoryPool* memory_pool = malloc(sizeof(MemoryPool));
  int dataBlockSize = (element_size * element_count);
  memory_pool->data_p = memory_pool->uninitialised_p = malloc(dataBlockSize);
  memory_pool->element_count = element_count;
  memory_pool->element_size = element_size;

  memory_pool->free_block_count = element_count;

  // next points to start of data block
  memory_pool->free_block_p = memory_pool->data_p;
  return memory_pool;

  /* memory_pool->numInitialized = 0; */
}

void free_memory_pool(MemoryPool* memory_pool) {
  free(memory_pool->data_p);
  free(memory_pool);
}

void* memory_pool_alloc(MemoryPool* memory_pool) {
  // Every time we call this alloc we initialise another block. We do that by
  // putting a pointer in it to the next sequential block
  if (memory_pool->uninitialised_p < memory_pool->end_of_data_p) {
    // Address of next uninitialised data
    uchar** uninitialised_p =
        memory_pool->uninitialised_p + memory_pool->element_size;
    // Put this address into where our uninitialised pointer points currently
    *memory_pool->uninitialised_p = (uchar*)uninitialised_p;
    // Set our unitialised pointer to this address as well.
    memory_pool->uninitialised_p = uninitialised_p;
  }

  void* free_block_p = NULL;
  // If we have any free blocks:
  if (memory_pool->free_block_count > 0) {
    // Return pointer to next free block
    free_block_p = (void*)memory_pool->free_block_p;
    // We have one less free block now
    --memory_pool->free_block_count;

    // If there are still free blocks:
    if (memory_pool->free_block_count > 0) {
      // Put whatever the pointer in the free block points to in our free_block
      // pointer
      memory_pool->free_block_p = (uchar**)(*memory_pool->free_block_p);
      /* index_to_address(memory_pool, *((uint*)memory_pool->free_block_p)); */
    } else {
      // NULL means no more unallocated blocks!!!
      memory_pool->free_block_p = NULL;
    }
  } else {
    // We ran out of space in the pool!!!!! Resize the fucker!!!
    int oldCount = memory_pool->element_count;
    int newCount = oldCount * 2;
    memory_pool->element_count = newCount;
    /* int dataBlockSize = (newCount * memory_pool->element_size); */
    /* int newSize = dataBlockSize + sizeof(MemoryPool); */
    int newSize = (newCount * memory_pool->element_size);
    printf("out of memory, reallocating as size %i\n", newSize);
    memory_pool = (MemoryPool*)realloc((void*)memory_pool, newSize);

    // Point offset at the first element of extra data block
    offset = memory_pool->free_offset =
        (oldCount + 1) * memory_pool->element_size;
    long int address = (long int)&memory_pool->data + offset;
    // Initialize new data block with pointers creating a linked list
    /* for (int i = oldCount; i < newCount - 1; i++) { */
    /*   offset += memory_pool->element_size; */
    /*   *(int*)address = offset; */
    /*   address += memory_pool->element_size; */
    /* } */
    /* *(int*)address = -1; */
    offset = memory_pool->free_offset;

    // rewrite memory_pool_addres's reference address
    *memory_pool->memory_pool_address = memory_pool;
  }
  return free_block_p;
}

void memory_pool_free(MemoryPool* memory_pool, void* block) {
  // There still free blocks
  if (memory_pool->free_block_p != NULL) {
    // Put the pointer to current next free block into this to be free block
    *(uchar**)block = (uchar*)memory_pool->free_block_p;
    // Point our free block pointer to the freed block
    memory_pool->free_block_p = block;
  } else
  // There no free blocks currently
  {
    *((uint**)block) = NULL;
    memory_pool->free_block_p = block;
  }
  ++memory_pool->free_block_count;
}

/* (*(uint*)block) = address_to_index(memory_pool,
 * memory_pool->free_block_p); */

/* if (memory_pool->numInitialized < memory_pool->element_count) { */
/*   // Element points to first free element/block */
/*   uint* element = */
/*       (uint*)index_to_address(memory_pool, memory_pool->numInitialized); */
/*   // Put index of which is now the next uninitialised block in it. */
/*   *element = memory_pool->numInitialized + 1; */
/*   // We have one more element initialised now. */
/*   memory_pool->numInitialized++; */
/* } */

/* uchar* index_to_address(MemoryPool* memory_pool, uint i) { */
/*   return (uchar*)(memory_pool->data_p + (i * memory_pool->element_size)); */
/* } */

/* uint address_to_index(MemoryPool* memory_pool, void* p) { */
/*   return (((uint)(p - memory_pool->data_p)) / memory_pool->element_size); */
/* } */
