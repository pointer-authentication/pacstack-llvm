; RUN: llc -aarch64-pacstack-ir-pass -pacstack=nomask -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s

; CHECK-LABEL: @func2
; CHECK: pacia
; CHECK-NOT: pacia
; CHECK: bl
; CHECK-NOT: pacia
; CHECK: autia
; CHECK: ret
define void @func2() #0 {
  call void @func1()
  ret void
}

declare void @func1()

attributes #0 = { nounwind readnone }

