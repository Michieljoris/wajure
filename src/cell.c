#include "cell.h"

#include "lispy_mempool.h"

Cell* make_cell() {
  Cell* cell = lalloc_type(CELL);
  cell->car = NULL;
  cell->cdr = NULL;
  return cell;
}

Cell* new_cell(void* car, void* cdr) {
  Cell* cell = lalloc_type(CELL);
  cell->car = car;
  cell->cdr = cdr;
  return cell;
}
