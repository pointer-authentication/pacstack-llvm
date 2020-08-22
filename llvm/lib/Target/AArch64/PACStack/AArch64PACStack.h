//===- AArch64PACStack.h - PAuth AArch64-specific stuff --------*- C++ -*--===//
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
// Defines some AArch64-specific variables for PACStack.
//
// TODO: Move DummyPA-only parts to AArch64DummyPA.cpp
//===----------------------------------------------------------------------===//

#ifndef LLVM_AARCH64PAFSSCOMMON_H
#define LLVM_AARCH64PAFSSCOMMON_H

#include "AArch64InstrInfo.h"
#include "AArch64Subtarget.h"

#include "llvm/PACStack/PACStack.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"

namespace llvm {
namespace PACStack {

static constexpr unsigned CR = AArch64::X28;
static constexpr unsigned CRSub = AArch64::W28;
static constexpr unsigned maskReg = AArch64::X15;

inline bool hasPACStackAttribute(const MachineFunction &MF) {
  const auto &F = MF.getFunction();

  if (F.hasFnAttribute(Attribute::AttrKind::NoReturn))
    return false; // Skip NoReturn functions (which might include main)

  if (!F.hasFnAttribute(PACStackAttribute))
    return false;

  return (F.getFnAttribute(PACStackAttribute).getValueAsString() != "none");
}

class AArch64PACStackCommon {
protected:
  const AArch64Subtarget *STI = nullptr;
  const AArch64InstrInfo *TII = nullptr;
  const AArch64RegisterInfo *TRI = nullptr;

  inline MachineInstrBuilder buildMOV(MachineBasicBlock &MBB,
                                      const DebugLoc &DL,
                                      MachineInstr *MI = nullptr) const;

  inline MachineInstrBuilder buildMOV(MachineBasicBlock &MBB,
                                      const DebugLoc &DL,
                                      unsigned dst,
                                      unsigned src,
                                      MachineInstr *MI = nullptr) const;

  inline MachineInstrBuilder buildAUTIA(MachineBasicBlock &MBB,
                                        const DebugLoc &DL,
                                        MachineInstr *MI = nullptr) const;

  inline MachineInstrBuilder buildAUTIA(MachineBasicBlock &MBB,
                                        const DebugLoc &DL,
                                        unsigned ptr,
                                        unsigned mod,
                                        MachineInstr *MI = nullptr) const;

  inline MachineInstrBuilder buildPACIA(MachineBasicBlock &MBB,
                                        const DebugLoc &DL,
                                        MachineInstr *MI = nullptr) const;

  inline MachineInstrBuilder buildPACIA(MachineBasicBlock &MBB,
                                        const DebugLoc &DL,
                                        unsigned ptr,
                                        unsigned mod,
                                        MachineInstr *MI = nullptr) const;

  inline MachineInstrBuilder buildEOR(MachineBasicBlock &MBB,
                                      const DebugLoc &DL,
                                      MachineInstr *MI = nullptr) const;

  inline MachineInstrBuilder buildEOR(MachineBasicBlock &MBB,
                                      const DebugLoc &DL,
                                      unsigned dst,
                                      unsigned src,
                                      MachineInstr *MI = nullptr) const;
};

MachineInstrBuilder AArch64PACStackCommon::buildMOV(MachineBasicBlock &MBB,
                                                    const DebugLoc &DL,
                                                    MachineInstr *MI) const {
  return MI != nullptr
         ? BuildMI(MBB, MI, DL, TII->get(AArch64::ORRXrs))
         : BuildMI(&MBB, DL, TII->get(AArch64::ORRXrs));
}


MachineInstrBuilder AArch64PACStackCommon::buildMOV(MachineBasicBlock &MBB,
                                                    const DebugLoc &DL,
                                                    unsigned dst,
                                                    unsigned src,
                                                    MachineInstr *MI) const {
  return buildMOV(MBB, DL, MI)
      .addReg(dst, RegState::Define)
      .addUse(AArch64::XZR)
      .addUse(src)
      .addImm(0);
}

MachineInstrBuilder AArch64PACStackCommon::buildAUTIA(MachineBasicBlock &MBB,
                                                      const DebugLoc &DL,
                                                      MachineInstr *MI) const {
  return MI != nullptr
         ? BuildMI(MBB, MI, DL, TII->get(AArch64::AUTIA))
         : BuildMI(&MBB, DL, TII->get(AArch64::AUTIA));
}

MachineInstrBuilder AArch64PACStackCommon::buildAUTIA(MachineBasicBlock &MBB,
                                                      const DebugLoc &DL,
                                                      unsigned ptr,
                                                      unsigned mod,
                                                      MachineInstr *MI) const {
  return buildAUTIA(MBB, DL, MI)
      .addReg(ptr, RegState::Define)
      .addUse(mod);
}

MachineInstrBuilder AArch64PACStackCommon::buildPACIA(MachineBasicBlock &MBB,
                                                      const DebugLoc &DL,
                                                      MachineInstr *MI) const {
  return MI != nullptr
         ? BuildMI(MBB, MI, DL, TII->get(AArch64::PACIA))
         : BuildMI(&MBB, DL, TII->get(AArch64::PACIA));
}

MachineInstrBuilder AArch64PACStackCommon::buildPACIA(MachineBasicBlock &MBB,
                                                      const DebugLoc &DL,
                                                      unsigned ptr,
                                                      unsigned mod,
                                                      MachineInstr *MI) const {
  return buildPACIA(MBB, DL, MI)
      .addReg(ptr, RegState::Define)
      .addUse(mod);
}

MachineInstrBuilder AArch64PACStackCommon::buildEOR(MachineBasicBlock &MBB,
                                                    const DebugLoc &DL,
                                                    MachineInstr *MI) const {
  return MI != nullptr
         ? BuildMI(MBB, MI, DL, TII->get(AArch64::EORXrs))
         : BuildMI(&MBB, DL, TII->get(AArch64::EORXrs));
}

MachineInstrBuilder AArch64PACStackCommon::buildEOR(MachineBasicBlock &MBB,
                                                    const DebugLoc &DL,
                                                    unsigned dst,
                                                    unsigned src,
                                                    MachineInstr *MI) const {
  return buildEOR(MBB, DL, MI)
      .addReg(dst, RegState::Define)
      .addReg(dst)
      .addReg(src)
      .addImm(0);
}

}
}

#endif //LLVM_AARCH64PAFSSCOMMON_H
