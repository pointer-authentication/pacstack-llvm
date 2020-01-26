//===----------------------------------------------------------------------===//
//
// Author: Hans Liljestrand <hans@liljestrand.dev>
// Copyright (c) 2019 Secure Systems Group, Aalto University https://ssg.aalto.fi/
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is class is for testing purposes only. It has no affect and only
// executes a bunch of asserts. Because the checks are behind asserts, this
// pass will do nothing on non-Debug builds.
//
// This should be removed when PACStack stabilitizes...

#include "PACStack/AArch64PACStack.h"

#include "AArch64.h"
#include "AArch64InstrInfo.h"
#include "AArch64Subtarget.h"

#include "llvm/PACStack/PACStack.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"

#define DEBUG_TYPE "AArch64PACStackPreEmitSanityChecker"

namespace llvm {
namespace PACStack {

class AArch64PACStackPreEmitSanityChecker : public MachineFunctionPass,
                                            public AArch64PACStackCommon {
public:
  static char ID;

  AArch64PACStackPreEmitSanityChecker() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  bool sanityCheckOk = true;

  bool sanityCheck(MachineFunction &MF);

  inline bool usesRegister(const MachineInstr &MI, const unsigned reg) {
    return MI.findRegisterUseOperandIdx(reg, false, TRI) != -1;
  }

  inline void doSingleCheck(const bool value, const std::string &msg) {
    if (!value) {
      sanityCheckOk = false;
      errs() << msg << "\n";
    }
  }
};

}
}

using namespace llvm;
using namespace llvm::PACStack;

char AArch64PACStackPreEmitSanityChecker::ID = 0;

FunctionPass *llvm::createAArch64PACStackPreEmitSanityChecker() {
  return new AArch64PACStackPreEmitSanityChecker();
}

bool AArch64PACStackPreEmitSanityChecker::runOnMachineFunction(MachineFunction &MF) {
  assert(!needsPACStack(MF) || sanityCheck(MF));
  return false;
}

bool AArch64PACStackPreEmitSanityChecker::sanityCheck(MachineFunction &MF) {
  int foundFrameSetupLR = 0;
  int foundFrameSetupCR = 0;
  int foundFrameDestroyCR = 0;

  for (auto &MBB : MF) {
    for (auto &MI : MBB) {
      if (isStore(MI) && MI.getFlag(MachineInstr::FrameSetup)) {
        if (usesRegister(MI, AArch64::LR))
          ++foundFrameSetupLR;
        if (usesRegister(MI, PACStack::CR))
          ++foundFrameSetupCR;

      } else if (isLoad(MI) && MI.getFlag(MachineInstr::FrameDestroy)) {
        if (MI.definesRegister(PACStack::CR))
            ++foundFrameDestroyCR;
      }
    }
  }

  doSingleCheck(foundFrameSetupCR == foundFrameSetupLR,
         "seem to be missing stores?");

  doSingleCheck(!(foundFrameSetupCR == 0 && foundFrameDestroyCR != 0),
         "found only FrameDestroy stuff!?!");

  doSingleCheck(!(foundFrameDestroyCR == 0 && foundFrameSetupCR != 0),
         "found only FrameSetup stuff!?!");

  if (sanityCheckOk)
    return true;

  MF.dump();
  return false;
}
