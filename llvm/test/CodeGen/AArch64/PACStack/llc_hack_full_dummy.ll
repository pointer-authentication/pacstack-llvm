; RUN: llc -aarch64-pacstack-ir-pass -aarch64-pacstack-dummy-pa -pacstack=full -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s

; CHECK-LABEL: @func2
; CHECK-NOT: pacia
; CHECK-DAG: eor x30
; CHECK-DAG: eor x30
; CHECK-DAG: eor x30
; CHECK-DAG: eor x30
; CHECK-DAG: mov x15, xzr
; CHECK: eor x15
; CHECK: eor x15
; CHECK: eor x15
; CHECK: eor x15
; CHECK: eor x30, x30, x15
; CHECK: mov x15, xzr
; CHECK-NOT: pacia
; CHECK: bl
; CHECK-NOT: pacia
; CHECK: mov x30, x28
; CHECK: ld
; CHECK: mov x15, xzr
; CHECK: eor x15
; CHECK: eor x15
; CHECK: eor x15
; CHECK: eor x15
; CHECK: eor x30
; CHECK: eor x30
; CHECK: eor x30
; CHECK: eor x30
; CHECK-NOT: autia
; CHECK: eor x30, x30, x28
; CHECK: mov x15, xzr
; CHECK: ret
define void @func2() #0 {
  call void @func1()
  ret void
}

declare void @func1()

attributes #0 = { nounwind readnone }
