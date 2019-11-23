#pragma once
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <sstream>
#include <string>
#include <map>

using namespace std;
using namespace llvm;
extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern Module TheModule;
extern map<string, Value*> NamedValues;


struct Ast {
  virtual void tostr(ostream& o) = 0;
  virtual Value* code() = 0;
  string str() { stringstream o; tostr(o); return o.str(); }
  friend ostream& operator<<(ostream& o, Ast& a) { a.tostr(o); return o; }
};

struct Num : Ast {
  double n;
  Num(double n) : n(n) {}
  void tostr(ostream& o) { o<<n; }
  Value* code() {
    return ConstantFP::get(TheContext, APFloat(n));
  }
};

struct Exp : Ast {
  char op;
  Ast *l, *r;
  Exp(char op, Ast *l, Ast *r) : op(op), l(l), r(r) {}
  void tostr(ostream& o) { o<<'('<<op<<' '<<*l<<' '<<*r<<')'; }
  Value* code() { switch(op) {
    case '+': return Builder.CreateFAdd(l->code(), r->code(), "add");
    case '-': return Builder.CreateFSub(l->code(), r->code(), "sub");
    case '*': return Builder.CreateFMul(l->code(), r->code(), "mul");
    case '/': return Builder.CreateFDiv(l->code(), r->code(), "div");
    default: return NULL;
  }}
};
