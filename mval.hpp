#pragma once
#include <string>
#include <map>

using namespace std;


enum Typ {BOL, INT, DEC, STR};
struct Val {
  Typ t;
  union {
    bool    b;
    long    i;
    double  d;
    char   *s;
  } v;

  Val() {}
  Val(bool x)   { t = BOL; v.b = x; }
  Val(long x)   { t = INT; v.i = x; }
  Val(double x) { t = DEC; v.d = x; }
  Val(char *x)  { t = STR; v.s = x; }
  bool b() { switch(t) {
    default:  return v.b;
  }}
  long i() { switch(t) {
    case BOL: return v.b;
    default:  return v.i;
  }}
  double d() { switch(t) {
    case BOL: return v.b;
    case INT: return v.i;
    default:  return v.d;
  }}
  string s() { switch (t) {
    case BOL: return v.b? "true" : "false";
    case INT: return to_string(v.i);
    case DEC: return to_string(v.d);
    default:  return v.s;
  }}
};
typedef map<string, Val> Env;
