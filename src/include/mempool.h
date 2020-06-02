#ifndef __MEMORY_POOL_H_
#define __MEMORY_POOL_H_

typedef unsigned int uint;
typedef unsigned char uchar;

typedef struct memory_pool MemPool;

struct memory_pool {
  uint total_slot_count;  // Num of blocks
  uint slot_size;         // Size of each block

  uint free_slot_count;  // Number of unallocated blocks
  void* free_slot_p;     // Address of next free block

  uint data_block_count;
  void** data_pointers;   // List of data pointers to all allocated data blocks
  void* end_of_data_p;    // Address of first byte *after* latest data block
  void* uninitialised_p;  // Pointer to remaining part of data that's not yet
                          // initialised in latest data block
  uint initialised_count;
  int auto_resize;

  // OBSOLETE: REMOTE
  void* data_p;  // Address of beginning of latest mallocated memory pool TODO:
                 // remove, obsolete
};

enum { MP_FIXED_SIZE, MP_AUTO_RESIZE };

MemPool* create_mempool(int slot_size, uint slot_clount, int auto_resize);

void free_mempool(MemPool* memory_pool);

void* mempool_alloc(MemPool* memory_pool);

void mempool_free(MemPool* memory_pool, void* block);

void mempool_debug(MemPool* mp);
#endif  // __MEMORY_POOL_H_
