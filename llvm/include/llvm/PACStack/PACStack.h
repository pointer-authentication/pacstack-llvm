//===----------------------------------------------------------------------===//
//
// Author: Hans Liljestrand <hans@liljestrand.dev>
// Copyright (c) 2020 Secure Systems Group, Aalto University https://ssg.aalto.fi/
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_PACSTACK_H
#define LLVM_PACSTACK_H

#include "llvm/ADT/StringRef.h"

namespace llvm {
namespace PACStack {

static const StringRef PACStackAttribute = "pacstack";

enum PACStackType {
  PACStackNone,
  PACStackFull,
  PACStackNoMask
};

bool isEnabled();
bool enableMasking();

}
}

#endif //LLVM_PACSTACK_H
