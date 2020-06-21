#include "lispy_mempool.h"

#include "io.h"
#include "lib.h"
#include "lval.h"
#include "malloc.h"
#include "mempool.h"
#include "print.h"

int debug = 0;

void mempool_debug(Mempool* mp) {
  /* printf("Total slot count: %i\n", mp->total_slot_count); */
  /* printf("Free slot count: %i\n", mp->free_slot_count); */
  /* printf("Data block count: %i\n", mp->data_block_count); */
  /* printf("Free slot pointer: %p\n", mp->free_slot_p); */
  /* printf("Initialised count: %i\n", mp->initialised_count); */
  if (debug) ddebug("%d | ", mp->total_slot_count - mp->free_slot_count);
}

Mempool** mempools;

typedef struct {
  int ref_count;
  Destructor destroy;
  int type;
  void* data_p;
} Slot;

// Alignment
#define PAD(siz) ((siz) + sizeof(int) - ((siz) % sizeof(int)))
#define SLOT_SIZE(type_size) PAD(sizeof(Slot)) + PAD(type_size)

void destroy_lval(void* data) {
  Lval* lval = (Lval*)data;

  if (debug)
    ddebug("destroying lval:%li (%s):", (long int)lval,
           lval_type_to_name(lval));
  if (debug) lval_debugln(lval);
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
        case LNIL:
        case LTRUE:
        case LFALSE:
          break;
        default:
          error("Can't delete unknown literal subtype: %d\n", lval->subtype);
      }
      break;
    case LVAL_FUNCTION:
      if (lval->subtype == SYS || lval->subtype == SPECIAL) {
        free(lval->func_name);
      } else {
        if (debug) ddebug("\n freeing params:");
        release(lval->params);
        if (debug) ddebug("\n freeing body:");
        release(lval->body);
        if (debug) ddebug("\n freeing closure_env:");
        /* if (debug) */
        /*   debug("ref count for closure_env = %d\n", */
        /*         get_ref_count(lval->closure_env)); */
        release(lval->closure_env);

        if (debug) ddebug("\n Done freeing lval_fun");
      }
      break;
    case LVAL_ERR:
      free(lval->err);
      break;
    default:
      error("Can't delete unknown type: %d\n", lval->type);
  }
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
Destructor destructors[SLOT_TYPE_COUNT] = {destroy_lval, destroy_lenv,
                                           destroy_cell, destroy_iter};

void lispy_mempool_log(int type, char* msg) {
  printf("%s %s", slot_type_str[type], msg);
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
MEMPOOL_LOG(char512, CHAR512);
MEMPOOL_LOG(char256, CHAR256);
MEMPOOL_LOG(char128, CHAR128);
MEMPOOL_LOG(char64, CHAR64);
MEMPOOL_LOG(char32, CHAR32);
MEMPOOL_LOG(char16, CHAR16);
MEMPOOL_LOG(char8, CHAR8);

#define MEMPOOL(typeint, type_size, type)                                \
  mempools[typeint] = create_mempool(SLOT_SIZE(type_size), type##_count, \
                                     MEMPOOL_AUTO_RESIZE, type##_mempool_log);

void init_lispy_mempools(uint lval_count, int lenv_count, int cell_count) {
  int char512_count = 10;
  int char256_count = 10;
  int char128_count = 10;
  int char64_count = 10;
  int char32_count = 10;
  int char16_count = 10;
  int char8_count = 10;
  int mempool_count = SLOT_TYPE_COUNT;
  mempools = _malloc(sizeof(Mempool*) * mempool_count);

  MEMPOOL(LVAL, sizeof(Lval), lval)
  MEMPOOL(LENV, sizeof(Lenv), lenv)
  MEMPOOL(CELL, sizeof(Cell), cell)
  MEMPOOL(ITER, sizeof(Cell), cell)
  MEMPOOL(CHAR512, 512, char512)
  MEMPOOL(CHAR256, 256, char256)
  MEMPOOL(CHAR128, 128, char128)
  MEMPOOL(CHAR64, 64, char64)
  MEMPOOL(CHAR32, 32, char32)
  MEMPOOL(CHAR16, 16, char16)
  MEMPOOL(CHAR8, 8, char8)
}

void free_lispy_mempools() {
  /* free_mempool(mempools[LVAL]); */
  /* free_mempool(mempools[LENV]); */
  /* free_mempool(mempools[CELL]); */
  /* free_mempool(mempools[ITER]); */
  /* free(mempools); */
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

int get_mempool_chartype(int size) {
  int type;
  if (size > MAX_CHAR_SIZE) {
    error("Can't allocate %d bytes, max is %d", size, MAX_CHAR_SIZE);
    type = CHAR512;
  } else {
    if (size & 512)
      type = CHAR512;
    else if (size & 128)
      type = CHAR128;
    else if (size & 64)
      type = CHAR64;
    else if (size & 32)
      type = CHAR32;
    else if (size & 16)
      type = CHAR16;
    else
      type = CHAR8;
  }
  return type;
}

void* lalloc_type(int type) {
  Slot* slot_p = mempool_alloc(mempools[type]);
  *slot_p = (Slot){.destroy = destructors[type],
                   .data_p = ((char*)slot_p + PAD(sizeof(Slot))),
                   .ref_count = 1,
                   .type = type};
  if (debug) ddebug("+%s: ", type_to_name(type));
  mempool_debug(mempools[type]);
  return slot_p->data_p;
}

void* lalloc_size(int size) {
  int type = get_mempool_chartype(size);
  return lalloc_type(type);
}

void lfree(int type, void* slot) {
  // Zeroing out will catch errors that happen when the slot is freed, but
  // still works as before because it's not been reassigned.
  memset(slot, 0, mempools[type]->slot_size);
  mempool_free(mempools[type], slot);
  if (debug) ddebug("-%s: ", type_to_name(type));
  mempool_debug(mempools[type]);
  /* printf("done %d\n", type); */
}

static Slot* get_slot_p(void* data_p) {
  return (Slot*)((char*)data_p - PAD(sizeof(Slot)));
}

void* lrealloc(void* data_p, int size) {
  Slot* slot = get_slot_p(data_p);
  int chartype = get_mempool_chartype(size);
  /* printf("old slot type=%d\n", slot->type); */
  /* printf("new char type=%d\n", chartype); */
  if (chartype == slot->type) return data_p;

  void* new_data_p = lalloc_type(chartype);
  int power_of_2 = chartype - CHAR8 + 3;
  int new_size = 1 << power_of_2;
  void* from_p = data_p;
  void* to_p = new_data_p;
  /* printf("%d new size=%d\n", power_of_2, new_size); */
  while (new_size--) copy_byte(from_p++, to_p++);
  release(data_p);
  return new_data_p;
}

int get_free_slot_count(int type) {
  Mempool* mp = mempools[type];
  return mp->free_slot_count;
}

int get_taken_slot_count(int type) {
  Mempool* mp = mempools[type];
  return mp->total_slot_count - mp->free_slot_count;
}

void print_mempool_free(int type) {
  printf("%s: %d ", type_to_name(type), get_taken_slot_count(type));
}

void print_mempool_counts() {
  printf("\n");
  print_mempool_free(LENV);
  print_mempool_free(LVAL);
  print_mempool_free(CELL);
  print_mempool_free(ITER);
}

int get_ref_count(void* data_p) { return get_slot_p(data_p)->ref_count; }

void* retain(void* data_p) {
  /* Slot* slot = get_slot_p(data_p); */
  /* printf("retain: %s-%li, ref count is %d\n", type_to_name(slot->type), */
  /*        (long int)data_p, slot->ref_count); */
  if (!data_p) {
    debug("Trying to retain NULL;\n");
  } else {
    ++get_slot_p(data_p)->ref_count;
  }
  return data_p;
}

int i = 0;
void release(void* data_p) {
  i++;
  if (!data_p) {
    /* printf("Warning: trying to release data with a NULL pointer."); */
    return;
  }
  Slot* slot = get_slot_p(data_p);

  if (data_p != slot->data_p) {
    /* printf("releasing:"); */
    /* lval_println(data_p); */
    warn("Warning: trying to release data that's not managed by ref_count.");
    exit(1);
    return;
  };

  /* printf("release: %s-%li, ref count is %d\n", type_to_name(slot->type), */
  /*        (long int)data_p, slot->ref_count); */
  if (--slot->ref_count) /* still referenced */ {
    if (slot->ref_count < 0) {
      warn("Warning: ref count for a %s has gone negative: %d\n",
           type_to_name(slot->type), slot->ref_count);
      lval_debugln(data_p);
      exit(1);
    }
    return;
  }
  int call_id = i;
  if (debug) {
    if (slot->type == LVAL) {
      ddebug("freeing: (i%d): ", i);
      lval_debugln(data_p);
    } else {
      ddebug("freeing: (i%d) %s: ", i, type_to_name(slot->type));
    }
  }
  if (slot->destroy) slot->destroy(data_p);
  if (debug) ddebug("\nactually now freeing slot for i%d ", call_id);
  lfree(slot->type, slot);
}

void clean_up(void* data) {
  if (debug) {
    ddebug("\nCleaning up: ");
    if (*(void**)data) {
      lval_debug(*(void**)data);
      ddebug("\n");
    } else
      ddebug("NIL\n");
  }
  release(*(void**)data);
}
