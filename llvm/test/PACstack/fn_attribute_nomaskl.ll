; RUN: opt -pacstack-opt -pacstack=nomask -S < %s | FileCheck %s

; CHECK-LABEL: @func
; CHECK: ret
define void @func() #0 {
  ret void
}

; CHECK: "pacstack"="nomask"
attributes #0 = { nounwind readnone }
