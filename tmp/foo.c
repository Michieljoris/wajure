#include <float.h>
#include <stdio.h>

typedef struct {
  int x;
  int y;
} point;

int main(int argc, char **argv) {
  printf("Hello World\n");
  point p;
  p.x = 1;
  p.y = 2;
  printf("%d + %d = %d", p.x, p.y, p.x + p.y);

  return 0;
}
