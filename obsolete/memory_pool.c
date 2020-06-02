/*
 * Adapted from:
 * https://www.semanticscholar.org/paper/Fast-Efficient-Fixed-Size-Memory-Pool%3A-No-Loops-and-Kenwright/4321a91d635d023ab25a743c698be219edcdb1a3
 * https://www.thinkmind.org/download.php?articleid=computation_tools_2012_1_10_80006
 * Resizing idea is from:
 * http://www.pinksquirrellabs.com/blog/2018/01/31/-fixed-memory-pool-design/
 *
 * */
#include "memory_pool.h"

#include <stdio.h>
#include <stdlib.h>

void* normalize_pointer(MemoryPool* memory_pool, void* pointer_or_offset) {
  if (memory_pool->auto_resize)
    pointer_or_offset =
        (int*)((long int)memory_pool->data_p + pointer_or_offset);
  return pointer_or_offset;
}

MemoryPool* ocreate_memory_pool(int element_size, uint element_count,
                                int auto_resize) {
  MemoryPool* memory_pool = malloc(sizeof(MemoryPool));
  memory_pool->auto_resize = auto_resize;
  int dataBlockSize = (element_size * element_count);
  memory_pool->data_p = memory_pool->uninitialised_p = malloc(dataBlockSize);

  void* data_p = auto_resize ? 0 : memory_pool->data_p;
  memory_pool->end_of_data_p = data_p + dataBlockSize;
  memory_pool->total_slot_count = element_count;
  memory_pool->slot_size = element_size;
  memory_pool->free_slot_count = element_count;

  // Free block pointer points to beginning of data block
  memory_pool->free_slot_p = data_p;
  return memory_pool;
}

void ofree_memory_pool(MemoryPool* memory_pool) {
  free(memory_pool->data_p);
  free(memory_pool);
}

void* omemory_pool_alloc(MemoryPool* memory_pool) {
  if (memory_pool->free_slot_count == 0) {
    if (memory_pool->auto_resize) {
      // We ran out of space in the pool!!!!! Resize the fucker!!!
      long int oldCount = memory_pool->total_slot_count;
      long int newCount = oldCount * 2;
      memory_pool->free_slot_count = newCount - memory_pool->total_slot_count;
      memory_pool->total_slot_count = newCount;
      long int dataBlockSize = (newCount * memory_pool->slot_size);
      printf("out of memory, reallocating as size %li\n", dataBlockSize);
      // Resize data
      memory_pool->data_p = realloc(memory_pool->data_p, dataBlockSize);
      // Point uninitialised and free block pointer at start of new data chunk
      memory_pool->uninitialised_p = memory_pool->free_slot_p =
          (uchar**)(oldCount * memory_pool->slot_size);
      // And update end of data pointer
      memory_pool->end_of_data_p = (uchar**)dataBlockSize;
    } else {
      printf("Out of memory, resizing is disabled!!!");
      return NULL;
    }
  }

  // Every time we call this alloc we initialise another block. We do that by
  // putting a pointer in it to the next sequential block
  if (memory_pool->uninitialised_p < memory_pool->end_of_data_p) {
    // Address (or offset) of next uninitialised data
    uchar** uninitialised_p =
        memory_pool->uninitialised_p + memory_pool->slot_size;

    uchar** real_uninitialised_p =
        (uchar**)normalize_pointer(memory_pool, memory_pool->uninitialised_p);
    // Put this address into where our uninitialised pointer points currently
    *real_uninitialised_p = (uchar*)uninitialised_p;
    // Set our unitialised pointer to this address as well.
    memory_pool->uninitialised_p = uninitialised_p;
  }

  // Get pointer to next free block
  void* free_block_p = (void*)memory_pool->free_slot_p;

  // If there are still free blocks:
  if (--memory_pool->free_slot_count > 0) {
    // Set the free block pointer to what its content points to
    uchar** real_free_block_p =
        (uchar**)normalize_pointer(memory_pool, memory_pool->free_slot_p);
    memory_pool->free_slot_p = (uchar**)(*real_free_block_p);
  } else {
    // Assign NULL to free block pointer when blocks have run out.
    memory_pool->free_slot_p = NULL;
  }
  return normalize_pointer(memory_pool, free_block_p);
}

void omemory_pool_free(MemoryPool* memory_pool, void* block) {
  // Guarded for NULL pointer, which free_block_p might be when not resizing.
  if (memory_pool->free_slot_p != NULL) {
    block = normalize_pointer(memory_pool, block);
    // Put the pointer to current next free block into this to be free block
    *(uchar**)block = (uchar*)memory_pool->free_slot_p;
  }

  // Point our free block pointer to the freed block
  memory_pool->free_slot_p = block;
  // We have one more free block
  ++memory_pool->free_slot_count;
}

// Pool might have been moved since initialising, so to get address of block
// this fn will have to be used if there's any chance of an allocation having
// happened in the meantime (if auto_resize).
void* memory_pool_get(MemoryPool* memory_pool, void* data_p) {
  data_p = normalize_pointer(memory_pool, data_p);
  // Calculate address of element at offset in pool
  return data_p;
}
