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
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#define DEBUG_TYPE "AArch64DummyPA"

using namespace llvm;
using namespace llvm::PACStack;

namespace {

class AArch64DummyPA : public MachineFunctionPass, private AArch64PACStackCommon {
public:
  static char ID;

  AArch64DummyPA() : MachineFunctionPass(ID) {}
  bool runOnMachineFunction(MachineFunction &) override;

private:
  bool convertBasicPAInstr(MachineBasicBlock &MBB, MachineInstr &MI);
};

}

char AArch64DummyPA::ID = 0;

FunctionPass *llvm::createAArch64DummyPA() {
  return new AArch64DummyPA();
}

bool AArch64DummyPA::runOnMachineFunction(MachineFunction &MF) {
  STI = &MF.getSubtarget<AArch64Subtarget>();
  TII = STI->getInstrInfo();
  TRI = STI->getRegisterInfo();

  bool changed = false;

  for (auto &MBB : MF) {
    for (auto MBBI = MBB.begin(), end = MBB.end(); MBBI != end; ) {
      auto &MI = *MBBI++; // update iterator here, since we might remove MI

      switch(MI.getOpcode()) {
        default:
          break;
        case AArch64::PACIA:
        case AArch64::PACIB:
        case AArch64::PACDA:
        case AArch64::PACDB:
        case AArch64::AUTIA:
        case AArch64::AUTIB:
        case AArch64::AUTDA:
        case AArch64::AUTDB:
          changed = convertBasicPAInstr(MBB, MI) | changed;
          break;
        case AArch64::PACGA:
        case AArch64::PACDZA:
        case AArch64::PACDZB:
        case AArch64::PACIZA:
        case AArch64::PACIZB:
        case AArch64::PACIA1716:
        case AArch64::PACIASP:
        case AArch64::PACIAZ:
        case AArch64::PACIB1716:
        case AArch64::PACIBSP:
        case AArch64::PACIBZ:
        case AArch64::AUTDZA:
        case AArch64::AUTDZB:
        case AArch64::AUTIZA:
        case AArch64::AUTIZB:
        case AArch64::AUTIA1716:
        case AArch64::AUTIASP:
        case AArch64::AUTIAZ:
        case AArch64::AUTIB1716:
        case AArch64::AUTIBSP:
        case AArch64::AUTIBZ:
          llvm_unreachable("unsupported");
      }
    }
  }

  return changed;
}

bool AArch64DummyPA::convertBasicPAInstr(MachineBasicBlock &MBB, MachineInstr &MI) {
  const auto &DL = MI.getDebugLoc();
  auto dst = MI.getOperand(0);
  auto mod = MI.getOperand(1);

  BuildMI(MBB, MI, DL, TII->get(AArch64::EORXri)).add(dst).add(dst).addImm(17);
  BuildMI(MBB, MI, DL, TII->get(AArch64::EORXri)).add(dst).add(dst).addImm(37);
  BuildMI(MBB, MI, DL, TII->get(AArch64::EORXri)).add(dst).add(dst).addImm(97);
  BuildMI(MBB, MI, DL, TII->get(AArch64::EORXrs)).add(dst).add(dst).add(mod).addImm(0);

  MI.removeFromParent();
  return true;
}
