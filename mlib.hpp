#pragma once
#include <string>

using namespace std;


extern "C" {
void fcls();
void fprintb(bool x);
void fprinti(long x);
void fprintd(double x);
void fprints(const char *x);
}
