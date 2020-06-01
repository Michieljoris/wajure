#ifndef __FMEMORY_POOL_H_
#define __FMEMORY_POOL_H_

typedef struct FMemoryPool {
  struct FMemoryPool** memory_pool_address;
  int element_count;
  int element_size;

  int num_free_elements;
  int num_initialized;

  int free_offset;

  void* data;
} FMemoryPool;

#endif  // __FMEMORY_POOL_H_
