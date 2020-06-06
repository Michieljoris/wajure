#include "lib.h"

typedef unsigned int uint;
#define NIL ((void *)0)

/* int strcmp2(const char *p1, const char *p2) { */
/*   const unsigned char *s1 = (const unsigned char *)p1; */
/*   const unsigned char *s2 = (const unsigned char *)p2; */
/*   unsigned char c1, c2; */
/*   do { */
/*     c1 = (unsigned char)*s1++; */
/*     c2 = (unsigned char)*s2++; */
/*     if (c1 == '\0') return c1 - c2; */
/*   } while (c1 == c2); */
/*   return c1 - c2; */
/* } */

/* uint strlen2(const char *str) { */
/*   int cnt; */
/*   asm("cld\n" /\* Search forward.  *\/ */
/*       /\* Some old versions of gas need `repne' instead of `repnz'.  *\/ */
/*       "repnz\n" /\* Look for a zero byte.  *\/ */
/*       "scasb"   /\* %0, %1, %3 *\/ */
/*       : "=c"(cnt) */
/*       : "D"(str), "0"(-1), "a"(0)); */
/*   return -2 - cnt; */
/* } */
