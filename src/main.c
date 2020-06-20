#include "binaryen.h"
#include "io.h"
#include "lib.h"
#include "run.h"
#include "test.h"

int main(int argc, char** argv) {
  /* long int a[] = {D(12), S("hello"), L(123)}; */
  /* out(a); */
  /* char* s1 = malloc(100); */
  /* _strcpy(s1, "foo"); */
  /* char* s2 = malloc(100); */
  /* _strcpy(s2, "bar"); */
  /* char* s3 = _strcat(s1, s2); */
  /* /\* printf("%s", _strcat("foo", "bar")); *\/ */
  /* free(s2); */
  /* printf("%s", s3); */
  /* free(s3); */
  /* long int foo = 123; */
  /* printf("%li", foo); */

  /* char* s = malloc(100); */
  /* printf("%s\n", itostr(s, LONG_MAX)); */

  /* printf("size %li\n", sizeof(long int)); */
  /* printf("size %li\n", sizeof(char*)); */
  /* make_bmodule(); */
  // Run lispy interpreterkk

  int size = 64 * 1024;
  char* p = malloc(size);
  char* p2;
  size += 64 * 1024;
  p2 = realloc(p, size);
  printf("%p, %p\n", p, p2);
  p = p2;
  size += 64 * 1024;
  p2 = realloc(p, size);
  printf("%p, %p\n", p, p2);
  p = p2;
  int i = 20;
  while (i--) {
    printf("%d\n", i);
    size += 64 * 1024;
    p2 = realloc(p, size);
    printf("%p, %p\n", p, p2);
    if (p2 != p) break;
    p = p2;
  }
  /* run(argc, argv); */
  return 0;
}
