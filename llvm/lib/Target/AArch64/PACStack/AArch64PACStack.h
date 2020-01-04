//===----------------------------------------------------------------------===//
//
// Author: Hans Liljestrand <hans@liljestrand.dev>
// Copyright (c) 2019 Secure Systems Group, Aalto University https://ssg.aalto.fi/
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
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
static constexpr unsigned maskReg = AArch64::X15;

static inline bool defsReg(const MachineBasicBlock &MBB, const unsigned reg) {
  for (const auto &MI : MBB)
    if (MI.findRegisterDefOperandIdx(reg) != -1)
      return true;
  return false;
}

static inline bool defsReg(const MachineFunction &MF, const unsigned reg) {
  for (const auto &MBB : MF)
    if (defsReg(MBB, reg))
      return true;
  return false;
}

inline bool doPACStack(MachineFunction &MF) {
  const auto &F = MF.getFunction();

  if (!F.hasFnAttribute(PACStackAttribute))
    return false;
  return (F.getFnAttribute(PACStackAttribute).getValueAsString() != "none");
}

inline bool doPACStackMasking(MachineFunction &MF) {
  const auto &F = MF.getFunction();

  if (!F.hasFnAttribute(PACStackAttribute))
    return false;
  return (F.getFnAttribute(PACStackAttribute).getValueAsString() == "full");
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


/*
class AArch64PACStackCommon {
protected:
  const AArch64Subtarget *STI = nullptr;
  const AArch64InstrInfo *TII = nullptr;
  const AArch64RegisterInfo *TRI = nullptr;

public:
  static inline bool useAutiaVariant() { return Pafss::useAutiaVariant(); };

  static inline bool useCollisionProtection() { return Pafss::useAraCp(); };

  static constexpr inline bool reserveAuthReg() { return true; };

  static constexpr inline bool dontCalleeSaveCR() { return false; };

  static constexpr inline unsigned getCR() { return AArch64::X28; };

  static constexpr inline unsigned getMaskReg() { return AArch64::X15; }

  static constexpr inline unsigned getSubCR() { return AArch64::W28; };

protected:

  inline void initRunOn(MachineFunction &MF);

  inline bool hasCalls(const MachineFunction &MF, bool ignoreTailCalls = false) const;

  inline MachineBasicBlock::iterator findFrameSetupEnd(MachineBasicBlock &MBB) const;

  inline bool defsLR(const MachineFunction &MF) const;

  inline bool defsReg(const MachineBasicBlock &MBB, unsigned reg) const;

  inline bool defsReg(const MachineFunction &MF, unsigned reg) const;


  inline const MachineInstr *findWeirdUseOfLR(MachineFunction &MF) const;
};

bool AArch64PACStackCommon::hasCalls(const MachineFunction &MF, bool ignoreTailCalls) const {
  for (const auto &MBB : MF)
    for (const auto &MI : MBB)
      if (MI.isCall() && (!ignoreTailCalls || MI.isTerminator()))
        return true;
  return false;
}

MachineBasicBlock::iterator AArch64PACStackCommon::findFrameSetupEnd(MachineBasicBlock &MBB) const {
  auto MBBI = MBB.end();
  --MBBI;

  for (const auto b = MBB.begin();
       MBBI != b && !MBBI->getFlag(MachineInstr::FrameSetup);
       --MBBI);

  return ++MBBI;
}

bool AArch64PACStackCommon::defsLR(const MachineFunction &MF) const {
  return defsReg(MF, AArch64::LR);
}

bool AArch64PACStackCommon::defsReg(const MachineBasicBlock &MBB, const unsigned reg) const {
  for (const auto &MI : MBB) {
    if (MI.findRegisterDefOperandIdx(reg) != -1)
      return true;
  }
  return false;
}

bool AArch64PACStackCommon::defsReg(const MachineFunction &MF, const unsigned reg) const {
  for (const auto &MBB : MF) {
    if (defsReg(MBB, reg))
      return true;
  }

  return false;
}



const llvm::MachineInstr *AArch64PACStackCommon::findWeirdUseOfLR(MachineFunction &MF) const {
  for (const auto &MBB : MF)
    for (const auto &MI : MBB)
      if (MI.findRegisterDefOperandIdx(AArch64::LR) != -1)
        if (!(MI.isCall() || MI.getFlag(MachineInstr::FrameDestroy) || MI.getFlag(MachineInstr::FrameSetup)))
          return &MI;
  return nullptr;
}
*/

}
}

#endif //LLVM_AARCH64PAFSSCOMMON_H
