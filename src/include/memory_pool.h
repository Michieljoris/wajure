#ifndef __MEMORY_POOL_H_
#define __MEMORY_POOL_H_

typedef struct MemoryPool {
  struct MemoryPool** memory_pool_address;
  int element_count;
  int element_size;

  int num_free_elements;
  int num_initialized;

  int free_offset;

  void* data;
} MemoryPool;

#endif  // __MEMORY_POOL_H_
