#include "refcount.h"

#include <stddef.h>

#include "io.h"
#include "lispy_mempool.h"
#include "ltypes.h"
#include "lval.h"
#include "print.h"

void mempool_debug(Mempool* mp);

// Allocates a new slot in the mempool.
void* alloc_rc_slot(Mempool* mempool, Destructor destroy) {
  Slot* slot_p = alloc_slot(mempool);
  *slot_p = (Slot){.destroy = destroy,
                   .data_p = ((char*)slot_p + PAD(sizeof(Slot))),
                   .ref_count = 1,
                   .mempool = mempool};
  /* if (debug) ddebug("+%s: ", type_to_name(type)); */
  /* mempool_debug(mp); */
  return slot_p->data_p;
}

void free_rc_slot(Slot* slot) {
  // Zeroing out will catch errors that happen when the slot is freed, but
  // still works as before because it's not been reassigned.
  Mempool* mp = slot->mempool;
  memset(slot, 0, mp->slot_size);
  free_slot(mp, slot);
  /* if (debug) ddebug("-%s: ", type_to_name(type)); */
  /* mempool_debug(mp); */
  /* printf("done %d\n", type); */
}

Slot* get_slot_p(void* data_p) {
  int slot_size = PAD(sizeof(Slot));
  return (Slot*)((char*)data_p - slot_size);
}

// Ups refcount for slot
void* retain(void* data_p) {
  /* Slot* slot = get_slot_p(data_p); */
  /* printf("retain: %s-%li, ref count is %d\n", type_to_name(slot->type), */
  /*        (long int)data_p, slot->ref_count); */
  if (!data_p) {
    /* debug("Trying to retain NULL;\n"); */
  } else {
    ++get_slot_p(data_p)->ref_count;
  }
  return data_p;
}

// Decrease refcount, free slot if 0
void release(void* data_p) {
  /* printf("release %li\n", (long)data_p); */
  /* lval_println((Lval*)data_p); */
  if (!data_p) {
    /* printf("Warning: trying to release data with a NULL pointer."); */
    return;
  }
  Slot* slot = get_slot_p(data_p);

  if (data_p != slot->data_p) {
    /* printf("releasing:"); */
    /* lval_println(data_p); */
    warn(
        "Warning: trying to release data that's not managed by ref_count "
        "(data_p: %li, slot->data_p: %li).\n",
        (long)data_p, (long)(slot->data_p));
    /* if (slot->mempool) { */
    /*   printf("mempool type: %d, %s\n", slot->mempool->type, */
    /*          type_to_name(slot->mempool->type)); */
    /* } */
#ifndef WASM
    exit(1);
#endif
    return;
  };

#ifdef WASM
    /* printf("sizeof lval: %li\n", sizeof(Lval)); */
    /* printf("release: type %s,slot_p: %li, data_p: %li, ref count is %d", */
    /*        type_to_name(slot->mempool->type), (long)slot, (long)data_p, */
    /*        slot->ref_count); */
    /* if (slot->mempool->type == LVAL) { */
    /*   printf(": "); */
    /*   lval_println((Lval*)data_p); */
    /* } else */
    /*   printf("\n"); */
#endif
  if (--slot->ref_count) /* still referenced */ {
    if (slot->ref_count < 0) {
      warn("Warning: ref count for a %s has gone negative: %d\n",
           type_to_name(slot->mempool->type), slot->ref_count);

      /* warn( */
      /*     "Warning: ref count for a mempool with type %d has gone negative: "
       */
      /*     "%d\n", */
      /*     slot->mempool->type, slot->ref_count); */
      /* lval_debugln(data_p); */
#ifndef WASM
      exit(1);
#endif
    }
    return;
  }
  /* if (debug) { */
  /*   if (slot->type == LVAL) { */
  /*     ddebug("freeing: (i%d): ", i); */
  /*     lval_debugln(data_p); */
  /*   } else { */
  /*     ddebug("freeing: (i%d) %s: ", i, type_to_name(slot->type)); */
  /*   } */
  /* } */
  /* printf("calling destroy on:\n"); */
  if (slot->destroy) slot->destroy(data_p);
  /* if (debug) ddebug("\nactually now freeing slot for i%d ", call_id); */
  free_rc_slot(slot);
}

void clean_up(void* data) {
  /* if (debug) { */
  /*   ddebug("\nCleaning up: "); */
  /*   if (*(void**)data) { */
  /*     lval_debug(*(void**)data); */
  /*     ddebug("\n"); */
  /*   } else */
  /*     ddebug("NIL\n"); */
  /* } */
  release(*(void**)data);
}

int get_free_slot_count(Mempool* mp) { return mp->free_slot_count; }

int get_taken_slot_count(Mempool* mp) {
  return mp->total_slot_count - mp->free_slot_count;
}

/* int get_free_slot_count(int type) { */
/*   Mempool* mp = mempools[type]; */
/*   return mp->free_slot_count; */
/* } */

/* int get_taken_slot_count(int type) { */
/*   Mempool* mp = mempools[type]; */
/*   return mp->total_slot_count - mp->free_slot_count; */
/* } */

int get_ref_count(void* data_p) {
  return data_p ? get_slot_p(data_p)->ref_count : -999;
}

void mempool_debug(Mempool* mp) {
  /* printf("Total slot count: %i\n", mp->total_slot_count); */
  /* printf("Free slot count: %i\n", mp->free_slot_count); */
  /* printf("Data block count: %i\n", mp->data_block_count); */
  /* printf("Free slot pointer: %p\n", mp->free_slot_p); */
  /* printf("Initialised count: %i\n", mp->initialised_count); */
  ddebug("%d | ", mp->total_slot_count - mp->free_slot_count);
}

/* char* serialize_value() { */

/* } */

void print_slot_size() {
  /* struct foo { */
  /*   char* p; */
  /*   int a; */
  /* }; */
  /* struct foo bar = {"hello foo", 1}; */
  printf("slot size: %li\n", sizeof(Slot));
  printf("char size: %li\n", sizeof(char));
  printf("lval size: %li, group %li\n", sizeof(Lval), offsetof(Lval, group));
  printf("lval size: %li, type %li\n", sizeof(Lval), offsetof(Lval, type));
  printf("lval size: %li, data %li\n", sizeof(Lval), offsetof(Lval, data));
  printf("lval size: %li, hash %li\n", sizeof(Lval), offsetof(Lval, hash));
  /* printf("lval size: %li, %li\n", sizeof(Lval), offsetof(Lval, fun)); */
  /* printf("lval size: %li, %li\n", sizeof(Lval), offsetof(Lval, closure)); */
  /* printf("lval size: %li, %li\n", sizeof(Lval), offsetof(Lval, params)); */
  /* printf("lval size: %li, %li\n", sizeof(Lval), offsetof(Lval, offset)); */
  /* printf("slot size: %li\n", sizeof(struct foo)); */
  /* printf("bar.a: %d\n", bar.a); */
  /* printf("bar.p: %p\n", bar.p); */
  /* /\* struct foo* p = &bar; *\/ */
  /* char* p = (char*)&bar; */
  /* printf("%s\n", p); */

  /* for (int i = 0; i < sizeof(struct foo); i++) printf("bar[%d]: %x\n", i,
   * p[i]); */
}
