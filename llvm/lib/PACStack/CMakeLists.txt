#===----------------------------------------------------------------------===//
#
# Author: Hans Liljestrand <hans@liljestrand.dev>
# Copyright (c) 2019 Secure Systems Group, Aalto University https://ssg.aalto.fi/
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===----------------------------------------------------------------------===//



add_llvm_library(PACStack
  PACStack.cpp

  DEPENDS
  LLVMCore
  LLVMSupport
  #LLVMCodeGen

  LINK_LIBS
  LLVMCore
  LLVMSupport
  #LLVMCodeGen
  )

set_property(TARGET PACStack PROPERTY LLVM_SYSTEM_LIBS)
