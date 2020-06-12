#ifndef __DEBUG_H_
#define __DEBUG_H_

#include "env.h"
#include "print.h"

void set_debug_level(int level);

int* pdebug_level;

void test_memory_pool();
void test_plist();
void test_list();

#define debug(m, lval)                         \
  printf("%s:%d: %s:", __FILE__, __LINE__, m); \
  if (lval) lval_print(lval);                  \
  puts("");

#endif  // __DEBUG_H_
