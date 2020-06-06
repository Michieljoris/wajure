#include "lispy_mempool.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "lval.h"
#include "mempool.h"

Mempool** mempools;

char* slot_types[] = {"LVAL", "LENV", "CELL"};

void lispy_mempool_log(int type, char* msg) {
  printf("%s %s", slot_types[type], msg);
}

#define MEMPOOL_LOG(TYPE, type)             \
  void TYPE##_mempool_log(char* fmt, ...) { \
    va_list va;                             \
    va_start(va, fmt);                      \
    char* msg = malloc(512);                \
    vsnprintf(msg, 511, fmt, va);           \
    msg = realloc(msg, strlen(msg) + 1);    \
    lispy_mempool_log(type, msg);           \
  }

MEMPOOL_LOG(lval, LVAL);
MEMPOOL_LOG(lenv, LENV);
MEMPOOL_LOG(cell, CELL);

void init_lispy_mempools(uint lval_count, int lenv_count, int cell_count) {
  mempools = malloc(sizeof(Mempool*) * 3);
  mempools[LVAL] = create_mempool(sizeof(Lval), lval_count, MEMPOOL_AUTO_RESIZE,
                                  lval_mempool_log);
  mempools[LENV] = create_mempool(sizeof(Lenv), lenv_count, MEMPOOL_AUTO_RESIZE,
                                  lenv_mempool_log);
  mempools[CELL] = create_mempool(sizeof(Cell), cell_count, MEMPOOL_AUTO_RESIZE,
                                  cell_mempool_log);
}

void free_lispy_mempools() {
  free_mempool(mempools[LVAL]);
  free_mempool(mempools[LENV]);
  free_mempool(mempools[CELL]);
  free(mempools);
}

void* lalloc(int type) { return mempool_alloc(mempools[type]); }
void lfree(int type, void* slot) { return mempool_free(mempools[type], slot); }
