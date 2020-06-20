#ifndef __PLATFORM_H_
#define __PLATFORM_H_

#ifdef WASM

extern void grow_memory(char* next_free_p);
extern char* get_memory(int size);
extern void* get_pointer_at(void** p);
extern void set_pointer_at(void** p1, void* p2);
extern void _putchar(char character);
extern void copy_byte(const char* from_p, char* to_p);

#else

#include <stdio.h>  //printf, puts
#include <stdlib.h>  //malloc, calloc, realloc

void grow_memory(char* next_free_p);
char* get_memory(int size);
void* get_pointer_at(void** p);
void set_pointer_at(void** p1, void* p2);

void copy_byte(const char* from_p, char* to_p);

void _putchar(char character);

#endif  // WASM

#endif  // __PLATFORM_H_
