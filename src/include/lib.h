#ifndef __LIB_H_
#define __LIB_H_

#include <stdarg.h>  //va_start, va_list

#include "platform.h"

#define LONG_MAX (long int)9223372036854775807
#define LONG_MIN (-LONG_MAX - 1L)
#define ERANGE 34
#define zize_t int

typedef unsigned int uint;
typedef unsigned char uchar;

extern int *merrno;

enum { TNULL, TSTRING, TLONG, TINT, TFLOAT, TUINT, TCHAR, TPTR };

/* typedef unsigned int zize_t; */
#define NIL ((void *)0)

zize_t _strlen(const char *s);

char *(_strcpy)(char *restrict s1, const char *restrict s2);

char *(_strcat)(char *restrict s1, const char *restrict s2);

char *(_strchr)(const char *s, int c);

int(_strcmp)(const char *s1, const char *s2);

char *(_strncat)(char *restrict s1, const char *restrict s2, zize_t n);

int(_strncmp)(const char *s1, const char *s2, zize_t n);

void *(memset)(void *s, int c, unsigned long n);

char *(_strncpy)(char *restrict s1, const char *restrict s2, zize_t n);

char *(_strrchr)(const char *s, int c);

int(memcmp)(const void *s1, const void *s2, unsigned long n);

char *(_strstr)(const char *haystack, const char *needle);

void *(_memmove)(void *s1, const void *s2, zize_t n);

long _strtol(const char *nptr, char **endptr, int base);

char *itostr(char str[], long int num);

long logical_rshift(long x, int n);

int is_eq_str(void *k1, void *k2);

#endif  // __LIB_H_
