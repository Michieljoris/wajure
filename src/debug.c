#include "debug.h"

int debug_level = 1;
int* pdebug_level = &debug_level;

void set_debug_level(int level) {
  /* debug->level = level; */
  *pdebug_level = level;
}
