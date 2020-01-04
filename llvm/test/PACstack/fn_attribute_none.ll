; RUN: opt -pacstack-opt -pacstack=none -S < %s | FileCheck %s

; CHECK-LABEL: @func
; CHECK: ret
define void @func() #0 {
  ret void
}

; CHECK-NOT: "pacstack"
attributes #0 = { nounwind readnone }
