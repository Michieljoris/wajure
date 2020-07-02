#ifndef __IO_H_
#define __IO_H_

#include "lib.h"

enum {
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARN,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_DEEP_DEBUG
};

void set_log_level(int level);
extern int log_level;

// TODO: change this macros so no code is inserted when not printing

#define _prefix(m) printf("%s (%s:%d): ", m, __FILE__, __LINE__);

#define log_filler(level, prefix)       \
  if (log_level >= LOG_LEVEL_##level) { \
  _prefix(prefix) printf(

#define error(...)                         \
  log_filler(ERROR, "ERROR") __VA_ARGS__); \
  }

#define warn(...)                           \
  log_filler(WARN, "WARNING") __VA_ARGS__); \
  }

#define info(...)                        \
  log_filler(INFO, "INFO") __VA_ARGS__); \
  }

#define debug(...)                         \
  log_filler(DEBUG, "DEBUG") __VA_ARGS__); \
  }

#define ddebug(...)                        \
  if (log_level >= LOG_LEVEL_DEEP_DEBUG) { \
    printf(__VA_ARGS__);                   \
  }

/* #define printf(...) \ */
/*   if (log_level >= LOG_LEVEL_INFO) printf(__VA_ARGS__); */

/* #define putchar(...) \ */
/*   if (log_level >= LOG_LEVEL_INFO) putchar(__VA_ARGS__); */

#endif  // __IO_H_
