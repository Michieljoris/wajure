#include "lispy_mempool.h"

#include "io.h"
#include "ltypes.h"
#include "lval.h"
#include "malloc.h"
#include "mempool.h"
#include "print.h"
#include "refcount.h"

/* int debug = 0; */

Mempool** mempools;

void* lalloc_type(int type);

void lispy_mempool_log(int type, char* msg);

#define MEMPOOL_LOG(TYPE, type)             \
  void TYPE##_mempool_log(char* fmt, ...) { \
    va_list va;                             \
    va_start(va, fmt);                      \
    char* msg = lalloc_type(INTERNAL);      \
    vsnprintf(msg, 511, fmt, va);           \
    msg = lrealloc(msg, _strlen(msg) + 1);  \
    lispy_mempool_log(type, msg);           \
    release(msg);                           \
  }

MEMPOOL_LOG(lval, LVAL);
MEMPOOL_LOG(lambda, FN);
MEMPOOL_LOG(lenv, LENV);
MEMPOOL_LOG(cell, CELL);
MEMPOOL_LOG(iter, ITER);
MEMPOOL_LOG(namespace, NAMESPACE);
MEMPOOL_LOG(internal, INTERNAL);
MEMPOOL_LOG(char512, CHAR512);
MEMPOOL_LOG(char256, CHAR256);
MEMPOOL_LOG(char128, CHAR128);
MEMPOOL_LOG(char64, CHAR64);
MEMPOOL_LOG(char32, CHAR32);
MEMPOOL_LOG(char16, CHAR16);
MEMPOOL_LOG(char8, CHAR8);

#define MEMPOOL(typeint, type_size, type)                         \
  mempools[typeint] =                                             \
      create_mempool(typeint, SLOT_SIZE(type_size), type##_count, \
                     MEMPOOL_AUTO_RESIZE, type##_mempool_log);

void init_lispy_mempools(uint lval_count, int lenv_count, int cell_count) {
  int namespace_count = 10;
  int lambda_count = 100;
  int internal_count = 1;  // just for the mempool log
  int char512_count = 10;
  int char256_count = 10;
  int char128_count = 10;
  int char64_count = 10;
  int char32_count = 40;
  int char16_count = 200;
  int char8_count = 1600;
  int mempool_count = SLOT_TYPE_COUNT;
  /* printf("malloc mempools count: %d\n", SLOT_TYPE_COUNT); */
  mempools = _malloc(sizeof(Mempool*) * mempool_count);

  // TODO wasm: if compiled to runtime we can reduce the size of the lval to 5 *
  // 4 bytes, since we don't need fun, closure, params, body, context and offset
  // attributes, we will never refer to it (by offset).
  MEMPOOL(LVAL, sizeof(Lval), lval)
  MEMPOOL(FN, sizeof(Lambda), lambda)
  // TODO wasm: we don't need Lenv either
  MEMPOOL(LENV, sizeof(Lenv), lenv)
  MEMPOOL(CELL, sizeof(Cell), cell)
  MEMPOOL(ITER, sizeof(Cell), cell)
  // TODO wasm: we don't need Namespace either
  MEMPOOL(NAMESPACE, sizeof(Namespace), namespace)
  MEMPOOL(INTERNAL, 512, internal)
  MEMPOOL(CHAR512, 512, char512)
  MEMPOOL(CHAR256, 256, char256)
  MEMPOOL(CHAR128, 128, char128)
  MEMPOOL(CHAR64, 64, char64)
  MEMPOOL(CHAR32, 32, char32)
  MEMPOOL(CHAR16, 16, char16)
  MEMPOOL(CHAR8, 8, char8)
}

void destroy_lval(void* data) {
  Lval* lval = (Lval*)data;

  /* if (debug) */
  /* printf("destroying lval: %li (%s):", (long int)lval,
   * lval_type_to_name(lval)); */
  /* lval_println(lval); */
  switch (lval->group) {
    case LVAL_SYMBOL:
      release(lval->data.str);
      break;
    case LVAL_COLLECTION:
      /* printf("releasing head\n"); */
      /* printf("head rc %d\n", get_ref_count(lval->head)); */
      /* printf("head-car rc %d\n", get_ref_count(lval->head->car)); */
      /* printf("head-cdr rc %d\n", get_ref_count(lval->head->cdr)); */
      release(lval->data.head);
      break;
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          break;
        case KEYWORD:
        case REGEX:
        case STRING:
          release(lval->data.str);
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
        release(lval->data.str);
      } else {
#ifdef WASM
#else
        release(lval->lambdas);
        release(lval->partials);
        release(lval->closure);
#endif  // WASM
      }
      break;

    case LVAL_REF:
      break;
    /* case LVAL_NAMESPACE: */
    /*   printf("releasing NAMESPACE\n"); */
    /*   release(lval->head); */
    /*   /\* TODO: Destroy namespace *\/ */
    /*   break; */
    case LVAL_ERR:
      /* free(lval->str); */
      release(lval->data.str);
      break;
    default:
      error("Can't delete unknown type: %d\n", lval->group);
  }
}

void destroy_lambda(void* lambda) {
  release(((Lambda*)lambda)->params);
  release(((Lambda*)lambda)->body);
}

void destroy_lenv(void* env) {
  release(((Lenv*)env)->kv);
  release(((Lenv*)env)->parent_env);
}

void destroy_cell(void* cell) {
  /* printf("destroy cell\n"); */

  /* printf("cell-car rc %d %li\n", get_ref_count(((Cell*)cell)->car), */
  /*        (long)(((Cell*)cell)->car)); */
  /* printf("cell-cdr rc %d %li\n", get_ref_count(((Cell*)cell)->cdr), */
  /*        (long)(((Cell*)cell)->cdr)); */
  release(((Cell*)cell)->car);
  release(((Cell*)cell)->cdr);
}
void destroy_iter(void* cell) {
  /* printf("destroying iter\n"); */
  release(((Cell*)cell)->car);
  release(((Cell*)cell)->cdr);
}

void destroy_namespace(void* namespace) {
  Namespace* ns = (Namespace*)namespace;
  release(ns->namespace);
  release(ns->env);
  release(ns->as);
  release(ns->refer);
}

// The order and number of these types needs to match with the enum in
// lispy_mempool.h!
char* slot_type_str[] = {"LVAL", "LAMBDA", "LENV", "CELL", "ITER", "NAMESPACE"};
Destructor destructors[SLOT_TYPE_COUNT] = {destroy_lval, destroy_lambda,
                                           destroy_lenv, destroy_cell,
                                           destroy_iter, destroy_namespace};

void lispy_mempool_log(int type, char* msg) {
  if (type >= CHAR8) {
    int power_of_2 = type - CHAR8 + 3;
    int size = 1 << power_of_2;
    printf("CHAR%d %s", size, msg);
    return;
  }
  printf("%s %s", slot_type_str[type], msg);
}

void free_lispy_mempools() {
  free_mempool(mempools[LVAL]);
  free_mempool(mempools[FN]);
  free_mempool(mempools[LENV]);
  free_mempool(mempools[CELL]);
  free_mempool(mempools[ITER]);
  free_mempool(mempools[NAMESPACE]);
  _free(mempools);
}

char* type_names[] = {"LVAL",      "LAMBDA",   "LENV",    "CELL",   "ITER",
                      "NAMESPACE", "INTERNAL", "CHAR8",   "CHAR16", "CHAR32",
                      "CHAR64",    "CHAR128",  "CHAR256", "CHAR512"};

char* type_to_name(int type) { return type_names[type]; }

int get_mempool_chartype(int size) {
  int type;
  if (size > MAX_CHAR_SIZE) {
    error("Can't allocate %d bytes, max is %d", size, MAX_CHAR_SIZE);
    type = CHAR512;
  } else {
    if (size & 512)
      type = CHAR512;
    else if (size & 256)
      type = CHAR512;
    else if (size & 128)
      type = CHAR256;
    else if (size & 64)
      type = CHAR128;
    else if (size & 32)
      type = CHAR64;
    else if (size & 16)
      type = CHAR32;
    else if (size & 8)
      type = CHAR16;
    else
      type = CHAR8;
  }
  return type;
}

void* lalloc_type(int type) {
  return alloc_rc_slot(mempools[type], destructors[type]);
}

// Allocates a new slot in the mempool.
void* lalloc_size(int size) {
  int type = get_mempool_chartype(size);
  return lalloc_type(type);
}

/* void copy_byte(const char* from_p, char* to_p) { *to_p = *from_p; } */

void* lrealloc(void* data_p, int size) {
  if (!data_p) return lalloc_size(size);
  Slot* slot = get_slot_p(data_p);
  int chartype = get_mempool_chartype(size);
  /* printf("old slot type=%d\n", slot->type); */
  /* printf("new char type=%d\n", chartype); */
  if (chartype == slot->mempool->type) return data_p;

  void* new_data_p = lalloc_type(chartype);
  int power_of_2 = chartype - CHAR8 + 3;
  int new_size = 1 << power_of_2;
  char* from_p = data_p;
  char* to_p = new_data_p;
  /* printf("%d new size=%d\n", power_of_2, new_size); */
  /* while (new_size--) copy_byte(from_p++, to_p++); */
  while (new_size--) *to_p++ = (*(char*)from_p++);
  release(data_p);
  return new_data_p;
}

void print_mempool_free(int type) {
  printf("%s: %d ", type_to_name(type), get_taken_slot_count(mempools[type]));
}

void print_mempool_counts() {
  printf("\n");
  print_mempool_free(LENV);
  print_mempool_free(LVAL);
  print_mempool_free(FN);
  print_mempool_free(CELL);
  print_mempool_free(ITER);
  print_mempool_free(NAMESPACE);
  print_mempool_free(INTERNAL);
  print_mempool_free(CHAR512);
  print_mempool_free(CHAR256);
  print_mempool_free(CHAR128);
  print_mempool_free(CHAR64);
  print_mempool_free(CHAR32);
  print_mempool_free(CHAR16);
  print_mempool_free(CHAR8);
}

SlotCount get_slot_count() {
  SlotCount ret = {.env = get_taken_slot_count(mempools[LENV]),
                   .lval = get_taken_slot_count(mempools[LVAL]),
                   .lambda = get_taken_slot_count(mempools[FN]),
                   .cell = get_taken_slot_count(mempools[CELL]),
                   .iter = get_taken_slot_count(mempools[ITER])};
  return ret;
}
