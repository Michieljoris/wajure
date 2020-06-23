#ifdef WASM
#include "binaryen.h"
#endif

#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "malloc.h"
#include "nargs.h"
#include "printf.h"
#include "run.h"
#include "test.h"

typedef struct {
  int type;
  union Value {
    int n;
    char* s;
  } v;
} foo;

/* #define NUMARGS(...) (sizeof((int[]){0, ##__VA_ARGS__}) / sizeof(int) - 1) */
#define NUMARGS(...) (sizeof((int[]){__VA_ARGS__}) / sizeof(int))

void bar(foo x) {
  switch (x.type) {
    case TINT:
      printf("%d, %d\n", x.type, x.v.n);
      break;
    case TSTRING:
      printf("%d, %s\n", x.type, x.v.s);
      break;
  };
}

void f1(foo a1) { bar(a1); }
void f2(foo a1, foo a2) {
  bar(a1);
  bar(a2);
}
/* void f3(foo a1, foo a2, foo a3) { printf("3\n"); } */
/* void f4(foo a1, foo a2, foo a3, foo a4) { printf("4\n"); } */

#define f(...) MKFN(f, ##__VA_ARGS__)

/* #define N(n)  */
/* foo int_(int t) { */
/*   foo x = {.type = TINT, .v.n = 123}; */
/*   return x; */
/* } */

/* foo str_(char* s) { */
/*   foo x = {.type = TSTRING, .v.s = s}; */
/*   return x; */
/* } */

int* parse_format_string(int* types, char* format) {
  int i = 0;
  while (*format) {
    if (*format != '%') {
      format++;
      continue;
    } else {
      format++;
    }

    if (*format == 's')
      types[i++] = TSTRING;
    else if (*format == 'l')
      types[i++] = TLONG;
    else if (*format == 'd' || *format == 'i')
      types[i++] = TINT;
    else if (*format == 'f')
      types[i++] = TFLOAT;
    else if (*format == 'p')
      types[i++] = TPTR;
    else if (*format == 'u')
      types[i++] = TUINT;
    else if (*format == 'c')
      types[i++] = TCHAR;
    else if (*format == '%')
      ;
    format++;
  }
  types[i++] = 0;
  /* printf("%d\n", i); */
  return types;
}

void _printf2(char* format, int n, ...) {
  /* int* types = malloc(16 * sizeof(int)); */
  int* types = lalloc_type(CHAR512);
  int* type = types;
  parse_format_string(types, format);
  /* while (*type) printf("%d ", *type++); */
  va_list va;
  va_start(va, n);
  for (int i = 0; i < n; ++i) {
    printf("%d: %d\n", *type++, va_arg(va, int));
  }
  va_end(va);
  release(types);
}

#define printf2(format, ...) \
  _printf2(format, PP_NARG(__VA_ARGS__), __VA_ARGS__);

int get_mempool_chartype2(int size) {
  int type;
  if (size > MAX_CHAR_SIZE) {
    error("Can't allocate %d bytes, max is %d", size, MAX_CHAR_SIZE);
    type = CHAR512;
  } else {
    if (size & 256)
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

int main(int argc, char** argv) {
  init_malloc(1, 10, 64 * 1024);
  /* somef(1, 2, 3); */
  /* char* format = "hello %s %l %i %d %f %u %c %p"; */
  /* printf("%d ", *types); */
  /* printf2(format, 1, 2, 3); */
  init_lispy_mempools(800, 800, 800);
  /* printf2("%d %s %p", 1, 2, 99999); */
  /* printf("%d\n", sizeof(char*)); */
  /* f(D(1), D(2)); */
  /* f(int_(123), str_("hello")); */
  /* char buf[256]; */
  /* char* buf = malloc(500); */
  /* stringf(buf, "foobar %d", 123); */
  /* char* buf = lalloc_size(512); */
  /* _strcpy(buf, "foo"); */
  /* printf("%d %s\n", _strlen(buf), buf); */
  /* char* buf2 = lrealloc(buf, 4); */
  /* printf("%d %s", _strlen(buf2), buf2); */
  /* test_printf(); */
  /* sprintf(buf, "hello"); */
  /* printf("%s\n", buf); */

  /* long int a[] = {D(12), S("hello"), L(123)}; */
  /* out(a); */
  /* printf("numarg: %li", NUMARGS((long)"a", 2, 3)); */
  /* printf("%d", (PP_NARG("a", b))); */
  /* make_bmodule(); */
  /* printf("%b , %b: ", 33, 300 & 256); */
  /* lalloc_size(300); */

  /* int i = 100; */
  /* while (i--) { */
  /*   lalloc_size(100); */
  /* } */

  /* int i = 20; */
  /* while (i--) { */
  /*   /\* make_lval_sym("foo"); *\/ */
  /*   printf(" [%d] ", 20 - i); */
  /*   lalloc_size(512); */
  /* } */

  // Run lispy interpreterkk
  run(argc, argv);

  free_malloc();
  return 0;
}
