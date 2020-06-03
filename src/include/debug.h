#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>

#include "env.h"
#include "print.h"

void set_debug_level(int level);

int* pdebug_level;

#define _printf(...) \
  if (*pdebug_level) printf(__VA_ARGS__);

/* #define _lval_print(...) \ */
/*   if (*pdebug_level) lval_print(__VA_ARGS__); */

/* #define _env_print(...) \ */
/*   if (*pdebug_level) env_print(__VA_ARGS__); */

#define _putchar(...) \
  if (*pdebug_level) putchar(__VA_ARGS__);

void test_memory_pool();
void test_plist();
#endif  // __DEBUG_H_
