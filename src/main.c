#ifndef WASM
#include "binaryen.h"
#include "binaryen_kitchen_sink.h"
#endif

#include <math.h>

#include "compile.h"
#include "compile_main.h"
#include "hash.h"
#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "malloc.h"
#include "nargs.h"
#include "print.h"
#include "printf.h"
#include "run.h"
#include "state.h"
#include "test.h"

/* 31 ^ (n + 1) + 31 ^ n* a_n + 31 ^ (n - 1) * a_n - 1 + … + 31 * a1 + a0 */
/* unsigned int vhash(unsigned int a0, unsigned int a1) { */
/* int result = calc_list_hash(1, a0); */
/* result = calc_list_hash(result, a1); */
/* return result; */
/* } */

void make_config(int argc, char** argv) {
  config->src = "clj";
  config->main = "run-main";
  config->stdlib = "wajure.core";
  config->user = "user";
  config->do_compile = 0;
  if (_strcmp(argv[1], "-c") == 0) {
    config->do_compile = 1;
    config->main = "main";
  }
}

int main(int argc, char** argv) {
  init_malloc(1, 10, 64 * 1024);
  init_lispy_mempools(3200, 3200, 3200);

  config = malloc(sizeof(Config));
  make_config(argc, argv);
  state = calloc(1, sizeof(State));

  for (int i = 2; i < argc; ++i) {
    if (config->do_compile) {
      printf("COMPILING!!!\n");
      compile_main();
    } else {
      printf("INTERPRETING!!!\n");
      run();
    }
  }
  printf("Back in main\n");
  free(state);
  free(config);
  free_lispy_mempools();
  free_malloc();
  printf("Exiting program\n");

  return 0;
}

/* test_types(); */
/* test_features(); */
/* test_core(); */
/* test_unreachable(); */
/* test_relooper(); */
/* test_binaries(); */
/* test_interpret(); */
/* test_nonvalid(); */
/* test_color_status(); */
/* test_for_each(); */

/* printf("%d\n", sizeof(int)); */
/* printf("%d\n", vhash(1, 2)); */
/* printf("%d\n", vhash(2, 1)); */
/* printf("%d\n", vhash(2, 2)); */
/* printf("%b\n", murmur3_int(1, 123)); */
/* printf("%b\n", murmur3_int(2, 123)); */
/* printf("%b\n", murmur3_int(3, 123)); */
/* printf("M: %u\n", murmur3_str("foo", 3, 123)); */
/* printf("pow: %x\n", power(3, 31)); */
/* printf("ipow: %x\n", ipow(3, 31)); */
/* printf("%u\n", 0xFFFFFFFF); */
/* printf("%u\n", 3 * my_pow(2, 31)); */
/* printf("%u\n", 1 * my_pow(2, 31)); */
/* printf("%d, %u\n", 1, long_hash_munge(1)); */
/* printf("%d, %d\n", 31457295, long_hash_munge(-1)); */
/* printf("%b\n", long_hash_munge(-2)); */
/* printf("%b\n", long_hash_munge(-3)); */
/* long a = -3; */
/* /\* unsigned long b = (unsigned long)a; *\/ */
/* long c = -3; */
/* printf("1: %lx\n   %lx\n", a, (unsigned long)a >> 7); */
/* printf("2: %lx\n   %lx\n", a, a >> 7 & 0x1FFFFFFFFFFFFFF); */
/* /\* printf("%lx\n %lx\n", a, logical_rshift(a, 7)); *\/ */
/* printf("%b, %d\n", 4 / 4, sizeof(long)); */
/* free_malloc(); */
/* signed int x1 = 5; */
/* printf("%d = %d\n", (x1 >> 1), 2); */
/* signed int x2 = -5; */
/* printf("%d = %d\n", (x2 >> 1), -3); */
/* unsigned int x3 = (unsigned int)-5; */
/* printf("%x = %x\n", (x3 >> 1), 0x7FFFFFFD); */
/* assert((x3 >> 1) == ); */
