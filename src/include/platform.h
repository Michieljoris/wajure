#ifndef __PLATFORM_H_
#define __PLATFORM_H_

// Use together with -Wl,--export-dynamic option for compiler (-Wl, means it
// goes to the linker as option)
#define export_wasm __attribute__((visibility("default")))

// Also works, but not as elegant:
/* #define export_wasm2(fn_name)  __attribute__((export_name("fn_name")))
 * fn_name */
// fn_name doesn't get interpolated because it's in quotes?

/* char get_byte_at(void* p); */
/* void set_byte_at(void* p1, char c); */

/* short get_word_at(void* p); */
/* void set_word_at(void* p1, short p2); */

/* int get_dword_at(void* p); */
/* void set_dword_at(void* p1, int p2); */

/* void copy_byte(const char* from_p, char* to_p); */

#ifdef WASM

char* get_memory();
void free_memory();

// imported into wasm module
void _putchar(char character);
char* get_mem_end();
void _putchar(char character);
int grow_memory();

// printf sends its output to a declared fn _putchar.
#include "printf.h"

// and now all regular putchar calls do as well.
#define putchar _putchar

#else

#include <stdio.h>  //printf, puts
#include <stdlib.h>  //malloc, calloc, realloc

// IO
void _putchar(char character);

// MEMORY
char* get_memory();
char* get_mem_end();
void free_memory();
int grow_memory();

#endif  // WASM

#endif  // __PLATFORM_H_
