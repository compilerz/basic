#pragma once
#include "mval.hpp"
#include <llvm/IR/Value.h>
#include <sstream>
#include <utility>
#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace llvm;
typedef pair<string, string> Def;


struct Ast {
  virtual void tostr(ostream& o) = 0;
  virtual Value* code() = 0;
  string str() { stringstream o; tostr(o); return o.str(); }
  friend ostream& operator<<(ostream& o, Ast& a) { a.tostr(o); return o; }
  // friend ostream& operator<<(ostream& o, Def d) {
  //   o<<d.first<<' '<<d.second;
  //   return o;
  // }
  friend ostream& operator<<(ostream& o, vector<Ast*> as) {
    const char *s = "";
    for (auto& a : as) { o<<s<<*a; s = " "; }
    return o;
  }
  // friend ostream& operator<<(ostream& o, vector<Def> ds) {
  //   const char *s = "";
  //   for (auto& d : ds) { o<<s<<d; s = ", "; }
  //   return o;
  // }
};

struct Line : Ast {
  Ast *x;
  string l;
  Line(Ast *x, string l) : x(x), l(l) {}
  void tostr(ostream& o) { if (l.size()) { o<<l<<": "; } o<<*x<<"\n"; }
  Value* code();
};

struct Declare : Ast {
  string x;
  vector<Def*> fs;
  Declare(string x, vector<Def*> fs) : x(x), fs(fs) {}
  void tostr(ostream& o) { o<<"(declare "<<x<<" ["<<"])"; }
  Value* code();
};

struct Proc : Ast {
  string x;
  vector<Def> fs;
  Ast *b;
  Proc(string x, vector<Def> fs, Ast *b) : x(x), fs(fs), b(b) {}
  void tostr(ostream& o) { o<<"(proc "<<x<<" ["<<"]\n"<<*b<<")"; }
  Value* code();
};

struct If : Ast {
  Ast *c, *t, *e;
  If(Ast *c, Ast *t, Ast *e) : c(c), t(t), e(e) {}
  void tos(ostream& o) { o<<"(if "<<*c<<" "<<*t<<" "<<*e<<")"; }
  Value* code();
};

struct Select : Ast {
  Ast *x;
  vector<Ast*> cs;
  Select(Ast *x, vector<Ast*> cs) : x(x), cs(cs) {}
  void tos(ostream& o) { o<<"(select\n"<<cs<<")"; }
  Value* code();
};

struct For : Ast {
  string x, n;
  Ast *f, *t, *s, *b;
  For(string x, Ast *f, Ast *t, string n, Ast *s, Ast *b) :
  x(x), n(n), f(f), t(t), s(s), b(b) {}
  void tos(ostream& o) { o<<"(for "<<x<<" ["<<*f<<" "<<*t<<"] "<<n<<" ["<<*s<<"]\n"<<*b<<")"; }
  Value* code();
};

struct While : Ast {
  Ast *c, *b;
  While(Ast *c, Ast *b) : c(c), b(b) {}
  void tos(ostream& o) { o<<"(while "<<*c<<"\n"<<*b<<")"; }
  Value* code();
};

struct Do : Ast {
  Ast *ce, *cx, *b;
  Do(Ast *ce, Ast *cx, Ast *b) : ce(ce), cx(cx), b(b) {}
  void tos(ostream& o) { o<<"(do "<<*ce<<"\n"<<*b<<" "<<*cx<<")"; }
  Value* code();
};

struct Exit : Ast {
  char f;
  Exit(char f) : f(f) {}
  void tos(ostream& o) { o<<"(exit "<<f<<")"; }
  Value* code();
};

struct Return : Ast {
  string l;
  Return(string l) : l(l) {}
  void tos(ostream& o) { o<<"(return "<<l<<")"; }
  Value* code();
};

struct Gosub : Ast {
  string l;
  Gosub(string l) : l(l) {}
  void tos(ostream& o) { o<<"(gosub "<<l<<")"; }
  Value* code();
};

struct Goto : Ast {
  string l;
  Goto(string l) : l(l) {}
  void tos(ostream& o) { o<<"(goto "<<l<<")"; }
  Value* code();
};

struct LineInput : Ast {
  string p, x;
  LineInput(string p, string x) : p(p), x(x) {}
  void tos(ostream& o) { o<<"(lineinput "<<p<<" "<<x<<")"; }
  Value* code();
};

struct Input : Ast {
  string p;
  vector<string> xs;
  Input(string p, vector<string> xs) : p(p), xs(xs) {}
  void tos(ostream& o) { o<<"(input "<<p<<" "<<")"; }
  Value* code();
};

struct Let : Ast {
  string x;
  Ast *e;
  Let(string x, Ast *e) : x(x), e(e) {}
  void tos(ostream& o) { o<<"(let "<<x<<" "<<e<<")"; }
  Value* code();
};

struct Case : Ast {
  Ast *f, *t;
  Case(Ast *f, Ast *t) : f(f), t(t) {}
  void tos(ostream& o) { o<<"(case "<<*f<<" "<<*t<<")"; }
  Value* code();
};


struct Call : Ast {
  string f;
  vector<Ast*> ps;
  Call(string f, vector<Ast*> ps) : f(f), ps(ps) {}
  void tostr(ostream& o) { o<<'('<<f<<' '<<ps<<')'; }
  Value* code();
};

struct Id : Ast {
  string x;
  Id(string x) : x(x) {}
  void tostr(ostream& o) { o<<x; }
  Value* code();
};

struct Litr : Ast {
  Val x;
  Litr(Val x) : x(x) {}
  void tostr(ostream& o) { o<<x.s(); }
  Value* code();
};
