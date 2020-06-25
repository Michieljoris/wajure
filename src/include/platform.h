#ifndef __PLATFORM_H_
#define __PLATFORM_H_

// Also works, but not as elegant:
/* #define export_wasm2(fn_name)  __attribute__((export_name("fn_name")))
 * fn_name */
// fn_name doesn't get interpolated because it's in quotes?

// IO
void _putchar(char character);

// MEMORY
char* get_memory();
void free_memory();
int grow_memory();
char* get_mem_end();
void* get_pointer_at(void** p);
void set_pointer_at(void** p1, void* p2);

void copy_byte(const char* from_p, char* to_p);

#include <stdarg.h>  //va_start, va_list

#define LONG_MAX (long int)9223372036854775807
#define LONG_MIN (-LONG_MAX - 1L)
#define ERANGE 34

typedef unsigned int uint;
typedef unsigned char uchar;

#ifndef WASM

#include <stdio.h>  //printf, puts
#include <stdlib.h>  //malloc, calloc, realloc
#define export_wasm

#else

// Use together with -Wl,--export-dynamic option for compiler (-Wl, means it
// goes to the linker as option)
#define export_wasm __attribute__((visibility("default")))

#include "printf.h"
#define putchar _putchar

#endif  // WASM

#endif  // __PLATFORM_H_
