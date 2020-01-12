; RUN: llc -aarch64-pacstack-ir-pass -aarch64-pacstack-dummy-pa -pacstack=nomask -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s
; XFAIL: *

; CHECK-LABEL: @func2
; CHECK-NOT: pacia
; CHECK: eor
; CHECK: eor
; CHECK: eor
; CHECK: eor
; CHECK-NOT: pacia
; CHECK: bl
; CHECK-NOT: pacia
; CHECK: eor
; CHECK: eor
; CHECK: eor
; CHECK: eor
; CHECK-NOT: autia
; CHECK: ret
define void @func2() #0 {
  call void @func1()
  ret void
}

declare void @func1()

attributes #0 = { nounwind readnone }

