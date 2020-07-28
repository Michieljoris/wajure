#ifndef __REFCOUNT_H_
#define __REFCOUNT_H_
#include "mempool.h"

typedef void (*Destructor)(void*);

typedef struct {
  int ref_count;
  Destructor destroy;
  Mempool* mempool;
  void* data_p;
} Slot;

// Alignment
#define PAD(siz) \
  ((siz % sizeof(int)) == 0 ? siz : (siz) + sizeof(int) - ((siz) % sizeof(int)))
#define SLOT_SIZE(type_size) PAD(sizeof(Slot)) + PAD(type_size)

void* alloc_rc_slot(Mempool* mp, Destructor destroy);
void free_rc_slot(Slot* slot);

Slot* get_slot_p(void* data_p);
void* retain(void* data_p);
void release(void* data_p);

#define scoped __attribute__((__cleanup__(clean_up)))
void clean_up(void* data);

int get_free_slot_count(Mempool* mp);
int get_taken_slot_count(Mempool* mp);
int get_ref_count(void* data_p);

void print_slot_size();

#endif  // __REFCOUNT_H_
