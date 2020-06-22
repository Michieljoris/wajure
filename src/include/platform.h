#ifndef __PLATFORM_H_
#define __PLATFORM_H_

#ifdef WASM

// IO
extern void _putchar(char character);

// MEMORY
extern char* get_memory(int size);
extern void* get_pointer_at(void** p);
extern void set_pointer_at(void** p1, void* p2);

extern void copy_byte(const char* from_p, char* to_p);

extern int page_size;
extern int max_page_count;
extern int initial_page_count;

#else

#include <stdio.h>  //printf, puts
#include <stdlib.h>  //malloc, calloc, realloc

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

#endif  // WASM

#endif  // __PLATFORM_H_
