//===- PACStack.cpp - PACStack common header -------------------*- C++ -*--===//
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
#ifndef LLVM_PACSTACK_H
#define LLVM_PACSTACK_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Pass.h"

namespace llvm {
namespace PACStack {

static const StringRef PACStackAttribute = "pacstack";

enum PACStackType {
  PACStackNone,
  PACStackFull,
  PACStackNoMask
};

Pass *createPACStackOptCallPass();

bool doAArch64IRPass();
bool doDummyPA();

}
}

#endif //LLVM_PACSTACK_H
