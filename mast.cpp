#include "mast.hpp"
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

using namespace std;
using namespace llvm;
extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern Module TheModule;
extern map<string, Value*> NamedValues;
extern map<string, Function*> NamedFunctions;


/*
    e AND e   { $$ = new Call("and", $1, $3); }
  | e OR e    { $$ = new Call("or", $1, $3); }
  | e XOR e   { $$ = new Call("xor", $1, $3); }
  | e IMP e   { $$ = new Call("imp", $1, $3); }
  | e EQV e   { $$ = new Call("eqv", $1, $3); }
  | NOT e     { $$ = new Call("not", $2); }
  | e EQ e    { $$ = new Call("=", $1, $3); }
  | e LT e    { $$ = new Call("<", $1, $3); }
  | e GT e    { $$ = new Call(">", $1, $3); }
  | e LE e    { $$ = new Call("<=", $1, $3); }
  | e GE e    { $$ = new Call(">=", $1, $3); }
  | e NE e    { $$ = new Call("<>", $1, $3); }
  | e MOD e   { $$ = new Call("mod", $1, $3); }
*/

Value* Call::code() {
  vector<Value*> vs;
  for (auto& p : ps) vs.push_back(p->code());
  if (f == "+0") return vs[0];
  if (f == "-0") return Builder.CreateNeg(vs[0]);
  // if (f == "^") return Builder.CreateCall()
  // if (f == "\\") return
  // if (f == "\/")
  if (f == "*") return Builder.CreateFMul(vs[0], vs[1], "mul");
  if (f == "-") return Builder.CreateFSub(vs[0], vs[1], "sub");
  if (f == "+") return Builder.CreateFAdd(vs[0], vs[1], "add");
  if (f == "mod") return Builder.CreateCall(NamedFunctions["fmod"], vs, "mod");
  return NULL;
}

Value* Id::code() {
  return NULL;
}

Value* Litr::code() { switch (x.t) {
  case BOL: return ConstantInt::get(TheContext, APInt(1, x.b()));
  case INT: return ConstantInt::get(TheContext, APInt(8*sizeof(long), x.i()));
  case DEC: return ConstantFP::get(TheContext, APFloat(x.d()));
  default:  return Builder.CreateGlobalStringPtr(StringRef(x.s().c_str()));
}}
