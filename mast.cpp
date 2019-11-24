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
  | e EQ e    { $$ = new Call("=", $1, $3); }
  | e LT e    { $$ = new Call("<", $1, $3); }
  | e GT e    { $$ = new Call(">", $1, $3); }
  | e LE e    { $$ = new Call("<=", $1, $3); }
  | e GE e    { $$ = new Call(">=", $1, $3); }
  | e NE e    { $$ = new Call("<>", $1, $3); }
*/

Value* Line::code() {
  return NULL;
}

Value* Declare::code() {
  return NULL;
}

Value* Proc::code() {
  return NULL;
}

Value* If::code() {
  return NULL;
}

Value* Select::code() {
  return NULL;
}

Value* For::code() {
  return NULL;
}

Value* While::code() {
  return NULL;
}

Value* Do::code() {
  return NULL;
}

Value* Exit::code() {
  return NULL;
}

Value* Return::code() {
  return NULL;
}

Value* Gosub::code() {
  return NULL;
}

Value* Goto::code() {
  return NULL;
}

Value* LineInput::code() {
  return NULL;
}

Value* Input::code() {
  return NULL;
}

Value* Let::code() {
  return NULL;
}

Value* Case::code() {
  return NULL;
}

Value* Call::code() {
  vector<Value*> vs;
  for (auto& p : ps) vs.push_back(p->code());
  if (f == "+0") return vs[0];
  if (f == "-0") return Builder.CreateNeg(vs[0]);
  if (f == "^")  return Builder.CreateCall(NamedFunctions["pow"], vs, "^");
  if (f == "\\") return Builder.CreateCall(NamedFunctions["floor"], {Builder.CreateFDiv(vs[0], vs[1], "\\")}, "floor");
  if (f == "/")  return Builder.CreateFDiv(vs[0], vs[1], "\\");
  if (f == "*") return Builder.CreateFMul(vs[0], vs[1], "mul");
  if (f == "-") return Builder.CreateFSub(vs[0], vs[1], "sub");
  if (f == "+") return Builder.CreateFAdd(vs[0], vs[1], "add");
  if (f == "mod") return Builder.CreateCall(NamedFunctions["mod"], vs, "mod");
  if (f == "not") return Builder.CreateNot(vs[0]);
  if (f == "eqv") return Builder.CreateNot(Builder.CreateXor(vs[0], vs[1]));
  if (f == "imp") return Builder.CreateOr(Builder.CreateNot(vs[0]), vs[1]);
  if (f == "xor") return Builder.CreateXor(vs[0], vs[1]);
  if (f == "or") return Builder.CreateOr(vs[0], vs[1]);
  if (f == "and") return Builder.CreateAnd(vs[0], vs[1]);
  return Builder.CreateCall(NamedFunctions[f], vs, f.c_str());
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
