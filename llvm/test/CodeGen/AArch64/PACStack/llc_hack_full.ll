; RUN: llc -aarch64-pacstack-ir-pass -pacstack=full -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s

; CHECK-LABEL: @func2
; CHECK: pacia  x30, x28
; CHECK: mov x15, xzr
; CHECK: pacia x15, x28
; CHECK: eor x30, x30, x15
; CHECK-DAG: mov x15, xzr
; CHECK-DAG: mov x28, x30
; CHECK: bl
; CHECK: mov x15, xzr
; CHECK: pacia x15, x28
; CHECK: eor x30, x30, x15
; CHECK: mov x15, xzr
; CHECK: autia x30, x28
; CHECK: ret
define void @func2() #0 {
  call void @func1()
  ret void
}

declare void @func1()

attributes #0 = { nounwind readnone }
