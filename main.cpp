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
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include "mval.hpp"
#include "mast.hpp"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace llvm;
extern Ast *yyroot;
extern int yyparse();

Function* declareFn(Typ r, string x, vector<Typ> fs, bool isVarArg=false);
Type* getType(Typ t);
TargetMachine* getMachine();
int compile(TargetMachine *m, string fobj);
int link(string fobj, string fout);

LLVMContext TheContext;
IRBuilder<> Builder(TheContext);
Module TheModule("BASIC", TheContext);
map<string, Value*> NamedValues;
map<string, Function*> NamedFunctions;


int main(int argc, char **argv) {
  printf("program:\n");
  yyparse();
  printf("log: yyroot = %p\n", (void*) yyroot);
  printf("%s\n", yyroot->str().c_str());

  NamedFunctions["cls"] = declareFn(DEC, "fcls", {});
  NamedFunctions["sin"] = declareFn(DEC, "sin", {DEC});
  NamedFunctions["cos"] = declareFn(DEC, "cos", {DEC});
  NamedFunctions["mod"] = declareFn(DEC, "fmod", {DEC, DEC});
  NamedFunctions["print/b"] = declareFn(DEC, "fprintb", {BOL});
  NamedFunctions["print/i"] = declareFn(DEC, "fprinti", {INT});
  NamedFunctions["print/d"] = declareFn(DEC, "fprintd", {DEC});
  NamedFunctions["print/s"] = declareFn(DEC, "fprints", {STR});
  // auto PF = declareFn(INT, "printf", {}, true);
  auto F = declareFn(DEC, "expression", {});
  BasicBlock *BB = BasicBlock::Create(TheContext, "entry", F);
  Builder.SetInsertPoint(BB);
  Builder.CreateRet(yyroot->code());
  verifyFunction(*F);

  FunctionType *PFT = FunctionType::get(Type::getInt32Ty(TheContext), true);
  Function *PF = Function::Create(PFT, Function::ExternalLinkage, "printf", &TheModule);
  FunctionType *MFT = FunctionType::get(Type::getVoidTy(TheContext), false);
  Function *MF = Function::Create(MFT, Function::ExternalLinkage, "main", &TheModule);
  BasicBlock *MBB = BasicBlock::Create(TheContext, "entry", MF);
  Builder.SetInsertPoint(MBB);
  // Value *fmt = ConstantDataArray::getString(TheContext, "%f\n");
  Value *fmt = Builder.CreateGlobalStringPtr(StringRef("%f\n"));
  Value *expr = Builder.CreateCall(F, {}, "callexpression");
  Builder.CreateCall(PF, {fmt, expr}, "callprintf");
  Builder.CreateRetVoid();
  TheModule.print(errs(), nullptr);
  auto m = getMachine();
  if (compile(m, "basic.o")) return 1;
  return link("basic.o", "basic.out");
}

Function* declareFn(Typ r, string x, vector<Typ> fs, bool isVarArg) {
  auto _r = getType(r);
  vector<Type*> _fs;
  for (auto& f : fs) _fs.push_back(getType(f));
  auto _x = FunctionType::get(_r,_fs, isVarArg);
  return Function::Create(_x, Function::ExternalLinkage, x, &TheModule);
}

Type* getType(Typ t) { switch (t) {
  case BOL: return Type::getInt1Ty(TheContext);
  case INT: return Type::getInt64Ty(TheContext);
  case DEC: return Type::getDoubleTy(TheContext);
  default:  return Type::getInt8PtrTy(TheContext);
}}

TargetMachine* getMachine() {
  auto triple = sys::getDefaultTargetTriple();
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
  string err;
  auto target = TargetRegistry::lookupTarget(triple, err);
  auto cpu = "generic";
  auto features = "";
  TargetOptions opt;
  auto rm = Optional<Reloc::Model>();
  auto m = target->createTargetMachine(triple, cpu, features, opt, rm);
  TheModule.setDataLayout(m->createDataLayout());
  TheModule.setTargetTriple(triple);
  return m;
}

int compile(TargetMachine *m, string fobj) {
  error_code err;
  raw_fd_ostream dest(fobj, err, sys::fs::F_None);
  if (err) {
    errs()<<"could not open file: "<<err.message();
    return 1;
  }
  auto type = TargetMachine::CGFT_ObjectFile;
  legacy::PassManager pass;
  if (m->addPassesToEmitFile(pass, dest, type)) {
    errs()<<"target machine cant emit object file";
    return 1;
  }
  pass.run(TheModule);
  dest.flush();
  return 0;
}

int link(string fobj, string fout) {
  char buff[256];
  snprintf(buff, sizeof(buff),
    "clang -o \"%s\" mlib.cpp \"%s\" -lm",
    fout.c_str(), fobj.c_str());
  return system(buff);
}


void yyerror(const char *s) {
  fprintf(stderr, "error: %s\n", s);
}
