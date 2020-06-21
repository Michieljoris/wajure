#include "platform.h"

#ifdef WASM

#else  // not WASM

void _putchar(char character) {
  // send char to console etc.
  putchar(character);
}

void copy_byte(const char* from_p, char* to_p) { *to_p = *from_p; }

#endif  // WASM
