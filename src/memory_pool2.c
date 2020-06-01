/*
 * Adapted from:
 * https://www.semanticscholar.org/paper/Fast-Efficient-Fixed-Size-Memory-Pool%3A-No-Loops-and-Kenwright/4321a91d635d023ab25a743c698be219edcdb1a3
 * https://www.thinkmind.org/download.php?articleid=computation_tools_2012_1_10_80006
 * Resizing idea is from, but adding extra data blocks instead of reallocing:
 * http://www.pinksquirrellabs.com/blog/2018/01/31/-fixed-memory-pool-design/
 *
 * */
#include <stdio.h>
#include <stdlib.h>

#include "memory_pool.h"

uint add_data_block(MemoryPool* memory_pool, uint extra_slot_count) {
  uint data_block_size = (memory_pool->slot_size * extra_slot_count);
  memory_pool->data_block_count++;
  memory_pool->data_pointers =
      realloc(memory_pool->data_pointers,
              sizeof(void*) * memory_pool->data_block_count);
  memory_pool->data_pointers[memory_pool->data_block_count - 1] =
      memory_pool->uninitialised_p = malloc(data_block_size);
  memory_pool->end_of_data_p = memory_pool->uninitialised_p + data_block_size;

  memory_pool->total_slot_count += extra_slot_count;
  memory_pool->free_slot_count = extra_slot_count;

  // Free block pointer points to beginning of data block
  memory_pool->free_slot_p = memory_pool->uninitialised_p;
  return data_block_size;
}

MemoryPool* create_memory_pool(int slot_size, uint slot_clount,
                               int auto_resize) {
  MemoryPool* memory_pool = malloc(sizeof(MemoryPool));
  memory_pool->auto_resize = auto_resize;
  memory_pool->slot_size = slot_size;
  memory_pool->total_slot_count = 0;
  memory_pool->initialised_count = 0;
  memory_pool->data_block_count = 0;
  memory_pool->data_pointers = NULL;
  add_data_block(memory_pool, slot_clount);
  return memory_pool;
}

void free_memory_pool(MemoryPool* memory_pool) {
  while (memory_pool->data_block_count--)
    free(memory_pool->data_pointers[memory_pool->data_block_count - 1]);
  free(memory_pool);
}

void* memory_pool_alloc(MemoryPool* memory_pool) {
  /* printf("ALLOC: Free slot count: %i\n", memory_pool->free_slot_count); */
  /* Resizing ========== */
  if (memory_pool->free_slot_count == 0) {
    if (memory_pool->auto_resize) {
      /* Ran out of space, adding extra data block */
      printf("MEMPOOL: out of memory, resizing to %i\n",
             add_data_block(memory_pool, memory_pool->total_slot_count * 2));
    } else {
      printf("MEMPOOL: out of memory, resizing is disabled!!!\n");
      return NULL;
    }
  }

  /* Initialising slots ========== */
  // Every time we call this alloc we initialise another slot. We do that by
  // putting a pointer in it to the next sequential slot
  if (memory_pool->uninitialised_p < memory_pool->end_of_data_p) {
    /* if (memory_pool->initialised_count == */
    /*     memory_pool->total_slot_count - memory_pool->free_slot_count) { */
    memory_pool->initialised_count++;
    // Address of next uninitialised data
    void** uninitialised_p =
        memory_pool->uninitialised_p + memory_pool->slot_size;
    // Put this address into where our uninitialised pointer points currently
    *(void**)memory_pool->uninitialised_p = uninitialised_p;
    // Set our unitialised pointer to this address as well.
    memory_pool->uninitialised_p = uninitialised_p;
  }

  /* Pointer juggling ========== */
  // Get pointer to next free block
  void* free_slot_p = (void*)memory_pool->free_slot_p;

  // If there are still free blocks:
  if (--memory_pool->free_slot_count > 0) {
    // Set the free block pointer to what its content points to
    memory_pool->free_slot_p = *(void**)memory_pool->free_slot_p;
  } else {
    // Assign NULL to free block pointer when blocks have run out.
    memory_pool->free_slot_p = NULL;
  }
  return free_slot_p;
}

void memory_pool_free(MemoryPool* memory_pool, void* block) {
  // Put the pointer to current next free block into this to be freed block
  *(void**)block = memory_pool->free_slot_p;
  // Point our free block pointer to the freed block
  memory_pool->free_slot_p = block;
  // We have one more free block
  ++memory_pool->free_slot_count;
}

void memory_pool_debug(MemoryPool* mp) {
  printf("Total slot count: %i\n", mp->total_slot_count);
  printf("Free slot count: %i\n", mp->free_slot_count);
  printf("Data block count: %i\n", mp->data_block_count);
  printf("Free slot pointer: %p\n", mp->free_slot_p);
  printf("End of data block pointer: %p\n", mp->end_of_data_p);
  printf("Initialised count: %i\n", mp->initialised_count);
}
