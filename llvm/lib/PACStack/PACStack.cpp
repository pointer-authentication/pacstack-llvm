//===----------------------------------------------------------------------===//
//
// Author: Hans Liljestrand <hans@liljestrand.dev>
// Copyright (c) 2020 Secure Systems Group, Aalto University https://ssg.aalto.fi/
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/PACStack/PACStack.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace PACStack;

static cl::opt<PACStackType> PACStackTypeOpt(
    "pacstack", cl::init(PACStackNone),
    cl::desc("PACStack"),
    cl::value_desc("mode"),
    cl::values(clEnumValN(PACStackNone, "none", "Disable PACStack"),
               clEnumValN(PACStackFull, "full", "Full PACStack with masking"),
               clEnumValN(PACStackNoMask, "nomask", "PACStack without masking")
    ));

static cl::opt<bool>
    EnableAArch64IRPass("aarch64-pacstack-ir-pass", cl::Hidden,
                        cl::desc("Do the PACStack IR Pass in target"),
                        cl::init(true));

bool llvm::PACStack::isEnabled() { return PACStackTypeOpt != PACStackNone; }
bool llvm::PACStack::enableMasking() { return PACStackTypeOpt == PACStackFull; }
bool llvm::PACStack::doAArch64IRPass() { return EnableAArch64IRPass; }

#define DEBUG_TYPE "PACStack"

namespace {

class PACStackOptCallPass : public FunctionPass {
public:
  static char ID;

  PACStackOptCallPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
  }

private:
  bool insertNeedSpillIntrinsic(Function &F);
};

}

char PACStackOptCallPass::ID = 0;

static RegisterPass<PACStackOptCallPass> X("pacstack-opt", "");

Pass *llvm::PACStack::createPACStackOptCallPass() { return new PACStackOptCallPass(); }

bool PACStackOptCallPass::runOnFunction(Function &F) {
  switch(PACStackTypeOpt) {
    case PACStackNone:
      return false;
    case PACStackFull:
      F.addFnAttr(PACStackAttribute, "full");
      return true;
    case PACStackNoMask:
      F.addFnAttr(PACStackAttribute, "nomask");
      return true;
  }
}
