#include "mast.hpp"
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
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
Value* createPrint(vector<Value*> vs);
AllocaInst* createAlloca(Function *f, Type *t, const string &x);



Value* Blk::code() {
  for (auto& x : xs) x->code();
  return ConstantFP::get(TheContext, APFloat(0.0));
}

Value* If::code() {
  auto cv = c->code();
  auto _f = Builder.GetInsertBlock()->getParent();
  auto _t = BasicBlock::Create(TheContext, "then", _f);
  auto _e = BasicBlock::Create(TheContext, "else");
  auto _z = BasicBlock::Create(TheContext, "endif");
  Builder.CreateCondBr(cv, _t, _e);
  Builder.SetInsertPoint(_t);
  t->code();
  Builder.CreateBr(_z);
  _t = Builder.GetInsertBlock();
  _f->getBasicBlockList().push_back(_e);
  Builder.SetInsertPoint(_e);
  e->code();
  Builder.CreateBr(_z);
  _e = Builder.GetInsertBlock();
  _f->getBasicBlockList().push_back(_z);
  Builder.SetInsertPoint(_z);
  // auto phi = Builder.CreatePHI(tv->getType(), 2, "ifval");
  // phi->addIncoming(tv, _t);
  // phi->addIncoming(ev, _e);
  return (new Nop())->code();
}

Value* While::code() {
  auto _f = Builder.GetInsertBlock()->getParent();
  auto _c = BasicBlock::Create(TheContext, "while", _f);
  auto _b = BasicBlock::Create(TheContext, "wbody");
  auto _z = BasicBlock::Create(TheContext, "wend");
  Builder.CreateBr(_c);
  Builder.SetInsertPoint(_c);
  auto cv = c->code();
  Builder.CreateCondBr(cv, _b, _z);
  _c = Builder.GetInsertBlock();
  _f->getBasicBlockList().push_back(_b);
  Builder.SetInsertPoint(_b);
  b->code();
  Builder.CreateBr(_c);
  _b = Builder.GetInsertBlock();
  _f->getBasicBlockList().push_back(_z);
  Builder.SetInsertPoint(_z);
  return (new Nop())->code();
}

Value* Nop::code() {
  return ConstantFP::get(TheContext, APFloat(0.0));
}

Value* Let::code() {
  Value *_x = NamedValues[x], *_e = e->code();
  if (!_x) NamedValues[x] = _x = createAlloca(NULL, _e->getType(), x);
  return Builder.CreateStore(_e, _x);
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
  if (f == "<>") return Builder.CreateFCmpONE(vs[0], vs[0], "<>");
  if (f == ">=") return Builder.CreateFCmpOGE(vs[0], vs[1], ">=");
  if (f == "<=") return Builder.CreateFCmpOLE(vs[0], vs[1], "<=");
  if (f == ">") return Builder.CreateFCmpOGT(vs[0], vs[1], ">");
  if (f == "<") return Builder.CreateFCmpOLT(vs[0], vs[1], "<");
  if (f == "=") return Builder.CreateFCmpOEQ(vs[0], vs[1], "=");
  if (f == "not") return Builder.CreateNot(vs[0]);
  if (f == "eqv") return Builder.CreateNot(Builder.CreateXor(vs[0], vs[1]));
  if (f == "imp") return Builder.CreateOr(Builder.CreateNot(vs[0]), vs[1]);
  if (f == "xor") return Builder.CreateXor(vs[0], vs[1]);
  if (f == "or") return Builder.CreateOr(vs[0], vs[1]);
  if (f == "and") return Builder.CreateAnd(vs[0], vs[1]);
  if (f == "print") return createPrint(vs);
  return Builder.CreateCall(NamedFunctions[f], vs, f.c_str());
}

Value* Id::code() {
  Value *a = NamedValues[x];
  return Builder.CreateLoad(a, x.c_str());
}

Value* Litr::code() { switch (x.t) {
  case BOL: return ConstantInt::get(TheContext, APInt(1, x.b()));
  case INT: return ConstantInt::get(TheContext, APInt(8*sizeof(long), x.i()));
  case DEC: return ConstantFP::get(TheContext, APFloat(x.d()));
  default:  return Builder.CreateGlobalStringPtr(StringRef(x.s().c_str()));
}}

Value* createPrint(vector<Value*> vs) {
  for (auto& v : vs) {
    string f = "print/s";
    auto t = v->getType();
    if (t->isDoubleTy()) f = "print/d";
    if (t->isIntegerTy()) f = "print/i";
    Builder.CreateCall(NamedFunctions[f], {v}, f);
  }
  return ConstantFP::get(TheContext, APFloat(0.0));
}

AllocaInst* createAlloca(Function *f, Type *t, const string &x) {
  f = f? f : Builder.GetInsertBlock()->getParent();
  IRBuilder<> b(&f->getEntryBlock(), f->getEntryBlock().begin());
  return b.CreateAlloca(t, 0, x.c_str());
}
