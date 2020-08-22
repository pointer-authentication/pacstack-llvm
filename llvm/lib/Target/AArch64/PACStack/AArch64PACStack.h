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

  return F.hasFnAttribute(PACStackAttribute) &&
         F.getFnAttribute(PACStackAttribute).getValueAsString() != "none";
}

}
}

#endif //LLVM_AARCH64PAFSSCOMMON_H
