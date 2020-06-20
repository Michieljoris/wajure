#include "io.h"

#include "lib.h"

int debug_level = LOG_LEVEL_INFO;
int *log_level = &debug_level;

void set_log_level(int level) { *log_level = level; }

#define I TINT,
#define D(d) TINT, d
#define L(l) TLONG, (long int)l
#define S(s) TSTRING, (long int)s

char *out_types[3];

char *out_to_str(int count, long int a[]);
void out_fn(int count, long int a[]);

#define myout(a) out_fn(sizeof(a) / sizeof(long int), a);

char *out_types[] = {"%s", "%li", "%d"};

char *out_to_str(int count, long int a[]) {
  char *result = malloc(512);
  result[0] = '\0';
  for (int i = 0; i < count; ++i) {
    long int type = a[i];
    switch (type) {
      case TSTRING:
        _strcat(result, (char *)a[++i]);
        break;
      case TLONG:
      case TINT:;
        char *s = malloc(200);
        s = itostr(s, a[++i]);
        _strcat(result, s);
        free(s);
        break;
    }
  }
  return result;
}

void out_fn(int count, long int a[]) {
  char *s = out_to_str(count, a);
  printf("%s", s);
  free(s);
}
