#pragma once
#include "mval.hpp"
#include <llvm/IR/Value.h>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace llvm;


struct Ast {
  virtual void tostr(ostream& o) = 0;
  virtual Value* code() = 0;
  string str() { stringstream o; tostr(o); return o.str(); }
  friend ostream& operator<<(ostream& o, Ast& a) { a.tostr(o); return o; }
  friend ostream& operator<<(ostream& o, vector<Ast*> as) {
    const char *s = "";
    for (auto& a : as) { o<<s<<*a; s = " "; }
    return o;
  }
};

struct Blk : Ast {
  vector<Ast*> xs;
  Blk(vector<Ast*> xs) : xs(xs) {}
  void tostr(ostream& o) { for (auto& x : xs) o<<*x<<"\n"; }
  Value* code();
};

struct Let : Ast {
  string x;
  Ast *e;
  Let(string x, Ast *e) : x(x), e(e) {}
  Let(Ast *x, Ast *e) : Let(x->str(), e) {}
  void tostr(ostream& o) { o<<"(let "<<x<<" "<<*e<<")"; }
  Value* code();
};

struct Call : Ast {
  string f;
  vector<Ast*> ps;
  Call(string f, vector<Ast*> ps) : f(f), ps(ps) {}
  Call(Ast *f, vector<Ast*> ps) : Call(f->str(), ps) {}
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
