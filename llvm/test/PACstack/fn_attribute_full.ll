; RUN: opt -pacstack-opt -pacstack=full -S < %s | FileCheck %s

; CHECK-LABEL: @func
; CHECK: ret
define void @func() #0 {
  ret void
}

; CHECK: "pacstack"="full"
attributes #0 = { nounwind readnone }
