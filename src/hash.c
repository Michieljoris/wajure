#include "hash.h"

#include "lib.h"

int long_hash(unsigned long a) { return (int)(a ^ ((a >> 32) + 1)); }

int long_hash_munge(unsigned long a) {
  a ^= (a << 13);
  a ^= (a >> 7);
  a ^= (a << 17);
  a ^= (a >> 32);
  return (int)a;
}

uint calc_list_hash(Cell *cons, Cell *head) {
  int head_hash = head ? head->hash : 1;
  return 31 * head_hash + ((Lval *)(cons->car))->hash;
}

uint rotl32(uint x, int r) { return (x << r) | (x >> (32 - r)); }

uint avalanche(uint h1) {
  h1 ^= h1 >> 16;
  h1 *= 0x85ebca6b;
  h1 ^= h1 >> 13;
  h1 *= 0xc2b2ae35;
  h1 ^= h1 >> 16;
  return h1;
}

int C1 = 0xcc9e2d51;
int C2 = 0x1b873593;
int C3 = 0xe6546b64;

uint murmur3_int(uint input, int seed) {
  uint k1 = rotl32(input * C1, 15) * C2;
  uint h1 = rotl32(seed ^ k1, 13) * 5 + C3;
  h1 ^= 4;
  return avalanche(h1);
}

uint murmur3_str(const char *key, uint len, uint seed) {
  uint h = 0;
  uint k = 0;
  char *d = (char *)key;  // 32 bit extract from `key'
  const uint *chunks = NULL;
  const char *tail = NULL;  // tail - last 8 bytes
  int i = 0;
  int l = len / 4;  // chunk length

  h = seed;

  chunks = (const uint *)(d + l * 4);  // body
  tail = (const char *)(d + l * 4);    // last 8 byte chunk of `key'

  // for each 4 byte chunk of `key'
  for (i = -l; i != 0; ++i) {
    // next 4 byte chunk of `key'
    k = chunks[i];

    // encode next 4 byte chunk of `key'
    k = rotl32(k * C1, 15) * C2;
    h = rotl32(h ^ k, 13) * 5 + C3;
  }

  k = 0;

  // remainder
  switch (len & 3) {  // `len % 4'
    case 3:
      k ^= (tail[2] << 16);
    case 2:
      k ^= (tail[1] << 8);

    case 1:
      k ^= tail[0];
      k = rotl32(k * C1, 15) * C2;
      h ^= k;
  }

  h ^= len;

  return avalanche(h);
}
