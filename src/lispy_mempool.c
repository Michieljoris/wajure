#include "lispy_mempool.h"

#include "io.h"
#include "lib.h"
#include "lval.h"
#include "mempool.h"

Mempool** mempools;

typedef struct {
  int ref_count;
  Destructor destroy;
  int type;
  void* data_p;
} Slot;

// Alignment
#define PAD(siz) ((siz) + sizeof(int) - ((siz) % sizeof(int)))
#define SLOT_SIZE(type) PAD(sizeof(Slot)) + PAD(sizeof(type))

void destroy_lval(void* data) {
  Lval* lval = (Lval*)data;
  /* printf("destroying lval:%li\n", (long int)lval); */
  /* lval_println(lval); */
  switch (lval->type) {
    case LVAL_SYMBOL:
      free(lval->sym);
      break;
    case LVAL_COLLECTION:
      release(lval->head);
      break;
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          break;
        case STRING:
          free(lval->str);
          break;
        default:
          printf("Can't delete unknown literal subtype: %d\n", lval->subtype);
      }
      break;
    case LVAL_FUNCTION:
      if (lval->subtype == SYS || lval->subtype == SPECIAL) {
        free(lval->func_name);
      } else {
        release(lval->closure_env);
        release(lval->params);
        release(lval->body);
      }
      break;
    case LVAL_ERR:
      free(lval->err);
      break;
    default:
      printf("Can't delete unknown type: %d\n", lval->type);
  }
  /* release(lval->tco_env); */
}
void destroy_lenv(void* env) {
  release(((Lenv*)env)->kv);
  release(((Lenv*)env)->parent_env);
}
void destroy_cell(void* cell) {
  release(((Cell*)cell)->car);
  release(((Cell*)cell)->cdr);
}
void destroy_iter(void* cell) {
  /* printf("destroying iter\n"); */
  release(((Cell*)cell)->car);
  release(((Cell*)cell)->cdr);
}

// The order and number of these types needs to match with the enum in
// lispy_mempool.h!
char* slot_type_str[] = {"LVAL", "LENV", "CELL", "ITER"};
Destructor destructors[] = {destroy_lval, destroy_lenv, destroy_cell,
                            destroy_iter};

void lispy_mempool_log(int type, char* msg) {
  /* printf("%s %s", slot_type_str[type], msg); */
}

#define MEMPOOL_LOG(TYPE, type)             \
  void TYPE##_mempool_log(char* fmt, ...) { \
    va_list va;                             \
    va_start(va, fmt);                      \
    char* msg = malloc(512);                \
    vsnprintf(msg, 511, fmt, va);           \
    msg = realloc(msg, _strlen(msg) + 1);   \
    lispy_mempool_log(type, msg);           \
  }

MEMPOOL_LOG(lval, LVAL);
MEMPOOL_LOG(lenv, LENV);
MEMPOOL_LOG(cell, CELL);
MEMPOOL_LOG(iter, ITER);

Mempool* create_lispy_mempool(int count) {
  return create_mempool(sizeof(Lval), count, MEMPOOL_AUTO_RESIZE,
                        lval_mempool_log);
}

#define MEMPOOL(typeint, Type, type)                                \
  mempools[typeint] = create_mempool(SLOT_SIZE(Type), type##_count, \
                                     MEMPOOL_AUTO_RESIZE, type##_mempool_log);

void init_lispy_mempools(uint lval_count, int lenv_count, int cell_count) {
  int mempool_count = SLOT_TYPE_COUNT;
  mempools = malloc(sizeof(Mempool*) * mempool_count);

  MEMPOOL(LVAL, Lval, lval)
  MEMPOOL(LENV, Lenv, lenv)
  MEMPOOL(CELL, Cell, cell)
  MEMPOOL(ITER, Cell, cell)
}

void free_lispy_mempools() {
  free_mempool(mempools[LVAL]);
  free_mempool(mempools[LENV]);
  free_mempool(mempools[CELL]);
  free_mempool(mempools[ITER]);
  free(mempools);
}

char* type_to_name(int type) {
  switch (type) {
    case LVAL:
      return "LVAL";
      break;
    case LENV:
      return "LENV";
      break;
    case CELL:
      return "CELL";
      break;
    case ITER:
      return "ITER";
      break;
    default:
      return "huuuuh???";
  }
}

void* lalloc(int type) {
  Slot* slot_p = mempool_alloc(mempools[type]);
  *slot_p = (Slot){.destroy = destructors[type],
                   .data_p = ((char*)slot_p + PAD(sizeof(Slot))),
                   .ref_count = 1,
                   .type = type};
  printf("+%s: ", type_to_name(type));
  mempool_debug(mempools[type]);
  return slot_p->data_p;
}

void lfree(int type, void* slot) {
  // Zeroing out will catch errors that happen when the slot is freed, but still
  // works as before because it's not been reassigned.
  memset(slot, 0, mempools[type]->slot_size);
  mempool_free(mempools[type], slot);
  printf("-%s: ", type_to_name(type));
  mempool_debug(mempools[type]);
  /* printf("done %d\n", type); */
}

int get_free_slot_count(int type) {
  Mempool* mp = mempools[type];
  return mp->total_slot_count - mp->free_slot_count;
}

void print_mempool_free(int type) {
  printf("%s: %d ", type_to_name(type), get_free_slot_count(type));
}

void print_mempool_counts() {
  printf("\n");
  print_mempool_free(LENV);
  print_mempool_free(LVAL);
  print_mempool_free(CELL);
  print_mempool_free(ITER);
}

static Slot* get_slot_p(void* data_p) {
  return (Slot*)((char*)data_p - PAD(sizeof(Slot)));
}

int get_ref_count(void* data_p) { return get_slot_p(data_p)->ref_count; }

void* retain(void* data_p) {
  /* Slot* slot = get_slot_p(data_p); */
  /* printf("retain: %s-%li, ref count is %d\n", type_to_name(slot->type), */
  /*        (long int)data_p, slot->ref_count); */
  if (!data_p) {
    printf("Trying to retain NULL;\n");
  } else {
    ++get_slot_p(data_p)->ref_count;
  }
  return data_p;
}

void release(void* data_p) {
  if (!data_p) {
    /* printf("Warning: trying to release data with a NULL pointer."); */
    return;
  }
  Slot* slot = get_slot_p(data_p);

  if (data_p != slot->data_p) {
    /* printf("releasing:"); */
    /* lval_println(data_p); */
    printf("Warning: trying to release data that's not managed by ref_count.");
    return;
  };

  /* printf("release: %s-%li, ref count is %d\n", type_to_name(slot->type), */
  /*        (long int)data_p, slot->ref_count); */
  if (--slot->ref_count) /* still referenced */ {
    if (slot->ref_count < 0) {
      printf("Warning: ref count for a %s has gone negative: %d\n",
             type_to_name(slot->type), slot->ref_count);
      lval_println(data_p);
    }
    return;
  }

  if (slot->destroy) slot->destroy(data_p);
  lfree(slot->type, slot);
}

void clean_up(void* data) {
  printf("\nCleaning up: ");
  if (*(void**)data)
    lval_print(*(void**)data);
  else
    printf("NIL\n");
  release(*(void**)data);
}
