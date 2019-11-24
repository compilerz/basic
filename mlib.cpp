#include "mlib.hpp"
#include <stdio.h>

using namespace std;


void fcls() {
  printf("\x1B[1;1H\x1B[2J");
}

void fprintb(bool x) {
  if (x) printf("true");
  else printf("false");
}

void fprinti(long x) {
  printf("%ld", x);
}

void fprintd(double x) {
  printf("%f", x);
}

void fprints(const char *x) {
  printf("%s", x);
}
