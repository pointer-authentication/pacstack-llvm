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

  inline MachineInstr *findFrameDestroyStart(MachineBasicBlock &MBB) const;
  inline MachineInstr *findFrameSetupStart(MachineBasicBlock &MBB) const;
  inline MachineInstr *findFrameSetupStart(MachineFunction &MF) const;
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
          //assert(!MI.findRegisterDefOperand(reg, false, TRI) && "tyring to do double load");
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

  auto *MI = findFrameSetupStart(MF);

  if (MI == nullptr) {
    LLVM_DEBUG(dbgs() << DEBUG_TYPE << ": FrameSetup NOT instrumented in "
                      << MF.getName() << "\n");
    return false;
  }

  auto *MBB = MI->getParent();
  assert(MBB != nullptr && "MI should always have parent!");

  // First calculate the new aret into LR and leave CR intact
  buildPACIA(*MBB, DL, AArch64::LR, CR, MI)
      .setMIFlag(MachineInstr::FrameSetup);
  if (doPACStackMasking(MF))
    insertCollisionProtection(*MBB, MI, MachineInstr::FrameSetup);

  // We should now have:
  //    x28 = aret_{i-1}  From stack
  //    LR  = aret_{1}    From stack
  // and can continue with normal FrameSetup to store:
  //     aret{i-1} from CR into x28 slot
  //     aret{i} from LR into frame record

  // Move through FrameSetup
  while (MI != nullptr && MI->getFlag(MachineInstr::FrameSetup)) {
    // Don't kill LR on store, we need it afterwards
    if (isStore(*MI) && MI->killsRegister(AArch64::LR)) {
      MI->clearRegisterKills(AArch64::LR, TRI);
    }

    MI = MI->getNextNode();
  }

  // Then, at end of FrameSetup, move aret_{i} to CR from LR
  buildMOV(*MBB, DL, CR, AArch64::LR, MI)
      .setMIFlag(MachineInstr::FrameSetup)
      // Kill LR here since we don't need it anymore
      ->addRegisterKilled(AArch64::LR, TRI);

  LLVM_DEBUG(dbgs() << DEBUG_TYPE << ": instrumenting FrameSetup of   "
                    << MF.getName() << "\n");
  return false;
}

bool AArch64PACStack::instrumentEpilogues(MachineFunction &MF) {
  bool changed = false;
  const DebugLoc DL;

  // This could be stupid, as there might be better ways to find FrameDestory.
  // I assume we cannot rely on isReturnBlock and that there might be multiple
  // FrameDestroy blocks within one function. Both assumptions might or might
  // not be true, but at least this should work...
  for (auto &MBB : MF) {
    auto *MI = findFrameDestroyStart(MBB);

    if (MI == nullptr)
      continue;

    // STore aret{i} in x15 before FrameDestroy
    buildMOV(MBB, DL, AArch64::X15, CR, MI)
        .setMIFlag(MachineInstr::FrameDestroy)
        ->addRegisterKilled(CR, TRI);

    // Then move to the end of FrameDestroy
    do {
      assert(!MI->isReturn());
      MI = MI->getNextNode();
    } while (MI != nullptr && MI->getFlag(MachineInstr::FrameDestroy));

    // We should now have:
    //    x15 = aret_{i}    Secure
    //    x28 = aret_{i-1}  From stack
    //    LR  = aret_{1}    From stack (ignored)

    // Lets move the secure aret_{i} into LR
    buildMOV(MBB, DL, AArch64::LR, AArch64::X15, MI).setMIFlag(MachineInstr::FrameDestroy);

    // Remove masking from LR if masking is enabled
    if (doPACStackMasking(MF))
      insertCollisionProtection(MBB, MI, MachineInstr::FrameDestroy);

    // Authenticate LR
    buildAUTIA(MBB, DL, AArch64::LR, CR, MI).setMIFlag(MachineInstr::FrameDestroy);

    LLVM_DEBUG(dbgs() << DEBUG_TYPE << ": instrumenting FrameDestroy of " << MF.getName() << "\n");
    changed = true;
  }

  if (!changed)
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

MachineInstr *AArch64PACStack::findFrameDestroyStart(MachineBasicBlock &MBB) const {
  for (auto &MBBI : MBB) {
    if (MBBI.getFlag(MachineInstr::FrameDestroy))
      return &MBBI;
  }

  return nullptr;
}


MachineInstr *AArch64PACStack::findFrameSetupStart(MachineBasicBlock &MBB) const {
  // Check if this MBB contains FrameSetup
  for (auto &MBBI : MBB) {
      if (MBBI.getFlag(MachineInstr::FrameSetup))
        return &MBBI;
  }

  // Continue looking into successors
  for (auto &successor : MBB.successors()) {
    auto *start = findFrameSetupStart(*successor);
    if (start != nullptr)
      return start;
  }
  return nullptr;
}

MachineInstr *AArch64PACStack::findFrameSetupStart(MachineFunction &MF) const {
  return findFrameSetupStart(MF.front());
}
