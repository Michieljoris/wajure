#include <stdio.h>

#include "debug.h"
#include "run.h"
#include "test.h"

#define NIL ((void *)0)
int main(int argc, char **argv) {
  /* test_memory_pool(); */
  /* test_plist(); */
  /* test_list(); */
  /* return 0; */

  set_debug_level(1);

  // Run lispy interpreter
  run(argc, argv);
  return 0;
}
