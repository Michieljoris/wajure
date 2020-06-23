#include "io.h"

#include "lib.h"

int log_level = LOG_LEVEL_INFO;
/* int *log_level = &_log_level; */

void set_log_level(int level) { log_level = level; }
int get_log_level() { return log_level; }
