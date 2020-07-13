#ifndef __HASH_H_
#define __HASH_H_
#include "lib.h"
#include "ltypes.h"

int long_hash(unsigned long a);

int long_hash_munge(unsigned long a);

uint murmur3_str(const char *key, uint len, uint seed);

uint murmur3_int(uint input, int seed);

uint calc_list_hash(Cell *cons, Cell *head);

#endif  // __HASH_H_
