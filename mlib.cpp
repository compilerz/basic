#include "mlib.h"
#include <stdio.h>

extern "C" {
double fcls() {
  printf("\x1B[1;1H\x1B[2J");
  return 0;
}
}
