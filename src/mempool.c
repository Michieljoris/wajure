#include "mempool.h"

#include "io.h"
#include "lib.h"
#include "malloc.h"
#include "platform.h"

/*
 * Adapted from:
 * https://www.semanticscholar.org/paper/Fast-Efficient-Fixed-Size-Memory-Pool%3A-No-Loops-and-Kenwright/4321a91d635d023ab25a743c698be219edcdb1a3
 * https://www.thinkmind.org/download.php?articleid=computation_tools_2012_1_10_80006
 * Resizing idea is from this blog post, but adding extra data blocks instead of
 * reallocing:
 * http://www.pinksquirrellabs.com/blog/2018/01/31/-fixed-memory-pool-design/
 *
 * */

uint add_data_block(Mempool* mempool, uint extra_slot_count) {
  uint data_block_size = (mempool->slot_size * extra_slot_count);
  int data_block_pointers_size = sizeof(void*) * mempool->data_block_count;
  mempool->data_block_count++;
  mempool->data_pointers =
      _realloc(mempool->data_pointers, data_block_pointers_size,
               data_block_pointers_size + sizeof(void*));
  mempool->data_pointers[mempool->data_block_count - 1] =
      mempool->uninitialised_p = _malloc(data_block_size);

  mempool->total_slot_count += extra_slot_count;
  mempool->free_slot_count = extra_slot_count;

  // Free block pointer points to beginning of data block
  mempool->free_slot_p = mempool->uninitialised_p;
  return data_block_size;
}

Mempool* create_mempool(int slot_size, uint slot_clount, int auto_resize,
                        Log log) {
  Mempool* mempool = _malloc(sizeof(Mempool));
  mempool->auto_resize = auto_resize;
  mempool->log = log;
  mempool->slot_size = slot_size;
  mempool->total_slot_count = mempool->initialised_count =
      mempool->data_block_count = 0;
  mempool->data_pointers = NULL;
  add_data_block(mempool, slot_clount);
  return mempool;
}

void free_mempool(Mempool* mempool) {
  while (mempool->data_block_count--)
    free(mempool->data_pointers[mempool->data_block_count]);
  free(mempool);
}

int c = 0;
// Only initialises a new slot when needed.
void* mempool_alloc(Mempool* mempool) {
  // Resizing
  if (mempool->free_slot_count == 0 && c++ < 30) {
    if (mempool->auto_resize) {
      printf("resizing\n");
      mempool->log("MEMPOOL: data block full, resizing from %i slots to %i\n",
                   mempool->total_slot_count, mempool->total_slot_count * 2);
      add_data_block(mempool, mempool->total_slot_count);
    } else {
      mempool->log("MEMPOOL: out of memory, resizing is disabled!!!\n");
      return NULL;
    }
  }

  // Pointer juggling
  void* free_slot_p;
  if (mempool->initialised_count ==
      mempool->total_slot_count - mempool->free_slot_count) {
    // Initialising another slot
    mempool->initialised_count++;
    // Our current free slot is our at uninitialised block
    free_slot_p = mempool->uninitialised_p;
    // Move both free slot and uninitialised pointers forward by one slot
    mempool->free_slot_p = mempool->uninitialised_p =
        mempool->uninitialised_p + mempool->slot_size;
  } else {
    free_slot_p = (void*)mempool->free_slot_p;
    // Set pool's next free slot pointer to dereferenced current free slot
    /* mempool->free_slot_p = *(void**)mempool->free_slot_p; */
    mempool->free_slot_p = get_pointer_at(mempool->free_slot_p);
  }

  mempool->free_slot_count--;
  return free_slot_p;
}

void mempool_free(Mempool* mempool, void* slot) {
  // Put the pointer to current next free slot into this to be freed slot
  /* *(void**)slot = mempool->free_slot_p; */
  set_pointer_at(slot, mempool->free_slot_p);
  // Point our free slot pointer to the freed slot
  mempool->free_slot_p = slot;
  // We have one more free slot!!
  ++mempool->free_slot_count;
}
