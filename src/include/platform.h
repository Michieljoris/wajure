#ifndef __PLATFORM_H_
#define __PLATFORM_H_

#ifdef WASM

// IO
extern void _putchar(char character);

// MEMORY
char* get_memory(int size);
void free_memory();
int grow_memory();
char* get_mem_end();
void* get_pointer_at(void** p);
void set_pointer_at(void** p1, void* p2);

void copy_byte(const char* from_p, char* to_p);

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
