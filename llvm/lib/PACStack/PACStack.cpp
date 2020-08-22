//===- PACStack.cpp - PACStack ---------------------------------*- C++ -*--===//
//
// Author: Hans Liljestrand <hans@liljestrand.dev>
// Copyright (c) 2020 Secure Systems Group, Aalto University
//                    <https://ssg.aalto.fi/>
// Copyright (c) 2020 Secure Systems Group, University of Waterloo
//                    <https://crysp.uwaterloo.ca/research/SSG>
//
// Released under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Registers PACStack command line options:
//     `-pacstack=(none|full|nomask)` for the PACStack instrumentaiton
//     `-aarch64-pacstack-dummy-pa`   for replacing PAuth instruction with
//                                    instructions that emulate the overhead
//                                    and behavior of expected PAuth HW.
//
// Also defines a simple opt PASS that simply adds a corresponding attribute
// to all Functions. This would eventually be replaced by the Clang frontend
// adding the attributes in the front-end.
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
    cl::values(
        clEnumValN(PACStackNone, "none", "Disable PACStack"),
        clEnumValN(PACStackFull, "full", "Full PACStack with masking"),
        clEnumValN(PACStackNoMask, "nomask", "PACStack without masking")
    ));

static cl::opt<bool>
    EnableAArch64DummyPA("aarch64-pacstack-dummy-pa", cl::Hidden,
                         cl::desc("Replace PACStack PA to the PA analog"),
                         cl::init(false));

bool llvm::PACStack::doAArch64IRPass() {
  return PACStackTypeOpt != PACStackNone;
}
bool llvm::PACStack::doDummyPA() { return EnableAArch64DummyPA; }

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
