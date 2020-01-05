//===----------------------------------------------------------------------===//
//
// Author: Hans Liljestrand <hans@liljestrand.dev>
// Copyright (c) 2019 Secure Systems Group, Aalto University https://ssg.aalto.fi/
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "PACStack/AArch64PACStack.h"

#include "AArch64.h"
#include "AArch64InstrInfo.h"
#include "AArch64Subtarget.h"

#include "llvm/PACStack/PACStack.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"

#define DEBUG_TYPE "AArch64PACStack"

namespace llvm {
namespace PACStack {

class AArch64PACStack : public MachineFunctionPass, public AArch64PACStackCommon {
public:
  static char ID;

  AArch64PACStack() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override;
  bool instrumentPrologue(MachineFunction &MF);
  bool instrumentEpilogues(MachineFunction &MF);

private:
  inline MachineInstr *replaceSpillOfLr(MachineBasicBlock &MBB, unsigned reg);
  inline MachineInstr *replaceLoadOfLr(MachineBasicBlock &MBB, unsigned reg);
  inline bool couldBeFrameSetup(const MachineInstr &pMI);

  inline void insertCollisionProtection(MachineBasicBlock &MBB, MachineInstr *pMI, const MachineInstr::MIFlag &flag);

  inline bool hasFrameSetup(const MachineBasicBlock *pMBB) const;
  inline MachineBasicBlock *findFrameSetupBlock(MachineFunction &MF) const;
};

}
}

using namespace llvm;
using namespace llvm::PACStack;

char AArch64PACStack::ID = 0;

bool AArch64PACStack::runOnMachineFunction(MachineFunction &MF) {
  if (! doPACStack(MF))
    return false;

  STI = &MF.getSubtarget<AArch64Subtarget>();
  TII = STI->getInstrInfo();
  TRI = STI->getRegisterInfo();

  bool changed = 0;

  changed |= instrumentPrologue(MF);
  changed |= instrumentEpilogues(MF);

  return changed;
}

FunctionPass *llvm::createAArch64PACStack() {
  return new AArch64PACStack();
}

bool AArch64PACStack::couldBeFrameSetup(const MachineInstr &pMI) {
  if (pMI.getFlag(MachineInstr::FrameDestroy))
    return false;

  return !pMI.isTerminator();
}

MachineInstr *AArch64PACStack::replaceSpillOfLr(MachineBasicBlock &MBB, const unsigned reg) {
  for (auto MBBI = MBB.begin(), E = MBB.end(); MBBI != E && couldBeFrameSetup(*MBBI); ++MBBI) {
    switch (MBBI->getOpcode()) {
      default:
        assert(!MBBI->findRegisterUseOperand(AArch64::LR, true, TRI) && "didn't expect LR use here!!!");
        break;
      case AArch64::STRXui:
      case AArch64::STRXpre:
      case AArch64::STPXpre:
      case AArch64::STPXi:
        if (auto pO = MBBI->findRegisterUseOperand(AArch64::LR, true, TRI)) {
          assert(!MBBI->findRegisterUseOperand(reg, false, TRI) && "tyring to do double store");
          pO->setReg(reg);
          return &*MBBI;
        }
    }
  }

  return nullptr;
}

MachineInstr *AArch64PACStack::replaceLoadOfLr(MachineBasicBlock &MBB, const unsigned reg) {
  for (auto &MI : MBB) {
    switch (MI.getOpcode()) {
      default:
        break;
      case AArch64::LDRXui:
      case AArch64::LDRXpost:
      case AArch64::LDPXi:
      case AArch64::LDPXpost:
        if (auto pO = MI.findRegisterDefOperand(AArch64::LR, false, TRI)) {
          assert(!MI.findRegisterDefOperand(reg, false, TRI) && "tyring to do double load");
          pO->setReg(reg);
          return &MI;
        }
    }
  }

  return nullptr;
}

bool AArch64PACStack::instrumentPrologue(MachineFunction &MF) {
  const DebugLoc DL;
  assert(TRI->isReservedReg(MF, CR) && "expecting CR to be reserved");
  // FIXME: should we fix LiveIns for CR?

  // First we need to find FrameSetup, which in some cases might be omitted
  if (auto pMBB = findFrameSetupBlock(MF)) {
    // Then we need to find the LR spill, if any
    if (auto pSpillMI = replaceSpillOfLr(*pMBB, CR)) {
      auto *const pEndOfAuth = pSpillMI->getNextNode();

      // Modify this:
      //
      //   STR CR          ; stack <- $aret_{i-1}$                            <- pSpillMI
      //   ...                                                                <- pEndOfAuth
      //
      // to:
      //
      //   STR CR          ; stack <- aretm{i-1}$                           <- pSpillMI
      //   PACIA LR, CR    ; LR <- aret{i} = pacia(ret{i}, aretm_{i-1})
      //                   ; if doing collision protection:
      //   MOV X15, XZR      ; X15 <- 0
      //   PACIA X15, CR     ; X15 <- mask = pacia(0, aretm{i-1})
      //   EOR LR, LR, X15   ; LR <- aretm{i} = aret{i} ^ mask{i-1}
      //   MOV X15, XZR      ; X15 <- 0
      //   MOV CR, LR      ; CR <- aretm{i}
      //   ...                                                                <- pEndOfAuth


      buildPACIA(*pMBB, DL, AArch64::LR, CR, pEndOfAuth).setMIFlag(MachineInstr::FrameSetup);
      if (doPACStackMasking(MF))
        insertCollisionProtection(*pMBB, pEndOfAuth, MachineInstr::FrameSetup);
      buildMOV(*pMBB, DL, CR, AArch64::LR, pEndOfAuth).setMIFlag(MachineInstr::FrameSetup);

      // FIXME: can we always expect only one FrameSetup?
      LLVM_DEBUG(dbgs() << DEBUG_TYPE << ": instrumenting FrameSetup of   " << MF.getName() << "\n");
      return true;
    }
  }

  assert(!defsReg(MF, CR) && "CR is used, but not saved?"); // FIXME: this is pretty heavy, maybe remove?

  LLVM_DEBUG(dbgs() << DEBUG_TYPE << ": FrameSetup NOT instrumented in " << MF.getName() << "\n");
  return false;
}

bool AArch64PACStack::instrumentEpilogues(MachineFunction &MF) {
  bool changed = false;
  const DebugLoc DL;

  for (auto &MBB : MF) {
    if (!MBB.isReturnBlock())
      continue;

    if (auto pLoadMI = replaceLoadOfLr(MBB, CR)) {
      auto *const pEndOfAuth = pLoadMI->getNextNode();

      // Modify this:
      //
      //   LDR CR          ; <- load CR from stack (pLoadMI)          <- pLoadMI
      //   ...                                                        <- pEndOfAuth
      //
      // to:
      //
      //   MOV LR, CR      ; LR <- aretm{i}
      //   LDR CR          ; aretm'{i-1} <- stack                      <- pLoadMI
      //                   ; if doing collision protection:
      //   MOV X15, XZR      ; X15 <- 0
      //   PACIA X15, CR     ; X15 <- mask'{i-1} = pacia(0, aretm{i-1})
      //   EOR LR, LR, X15   ; LR <- aret'{i} = aretm{i} ^ mask'{i-1}
      //   MOV X15, XZR      ; X15 <- 0
      //   AUTIA LR, CR    ; LR <- ret{i} or ret*{i}
      //   ...                                                        <- pEndOfAuth

      assert(MBB.getLastNonDebugInstr() != MBB.end() && MBB.getLastNonDebugInstr()->isReturn() && "should find return");
      assert(pLoadMI->getNextNode() != nullptr && "next node should always be non-null");

      buildMOV(MBB, DL, AArch64::LR, CR, pLoadMI).setMIFlag(MachineInstr::FrameDestroy);
      if (doPACStackMasking(MF))
        insertCollisionProtection(MBB, pEndOfAuth, MachineInstr::FrameDestroy);
      buildAUTIA(MBB, DL, AArch64::LR, CR, pEndOfAuth).setMIFlag(MachineInstr::FrameDestroy);

      LLVM_DEBUG(dbgs() << DEBUG_TYPE << ": instrumenting FrameDestroy of " << MF.getName() << "\n");
      changed = true;
    }
  }

  LLVM_DEBUG(if (!changed) dbgs() << DEBUG_TYPE << ": NO FrameDestroy instrumented in " << MF.getName() << "\n");
  return changed;
}

inline void AArch64PACStack::insertCollisionProtection(MachineBasicBlock &MBB,
                                                       MachineInstr *pMI,
                                                       const MachineInstr::MIFlag &flag) {
  const DebugLoc DL;

  // MOV X15, XZR      ; X15 <- 0
  // PACIA X15, CR     ; X15 <- mask'{i-1} = pacia(0, aretm{i-1})
  // EOR LR, LR, X15   ; LR <- aret'{i} = aretm{i} ^ mask'{i-1}
  // MOV X15, XZR      ; X15 <- 0
  buildMOV(MBB, DL, maskReg, AArch64::XZR, pMI).setMIFlag(flag);
  buildPACIA(MBB, DL, maskReg, CR, pMI).setMIFlag(flag);
  buildEOR(MBB, DL, AArch64::LR, maskReg, pMI).setMIFlag(flag);
  buildMOV(MBB, DL, maskReg, AArch64::XZR, pMI).setMIFlag(flag);
}

bool AArch64PACStack::hasFrameSetup(const MachineBasicBlock *const pMBB) const {
  assert(pMBB != nullptr);
  for (const auto &MBBI : *pMBB)
    if (MBBI.getFlag(MachineInstr::FrameSetup))
      return true;
  return false;
}

MachineBasicBlock *AArch64PACStack::findFrameSetupBlock(MachineFunction &MF) const {
  for (auto &MBB : MF)
    if (hasFrameSetup(&MBB))
      return &MBB;
  return nullptr;
}

