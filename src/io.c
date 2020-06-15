#include "io.h"

int debug_level = LOG_LEVEL_INFO;
int* log_level = &debug_level;

void set_log_level(int level) { *log_level = level; }
