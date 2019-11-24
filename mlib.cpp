#include "mlib.h"
#include <stdio.h>

double fcls() {
  printf("\x1B[1;1H\x1B[2J");
  return 0;
}

