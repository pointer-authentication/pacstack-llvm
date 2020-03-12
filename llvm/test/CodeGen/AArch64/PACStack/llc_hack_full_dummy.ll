; RUN: llc -aarch64-pacstack-dummy-pa -pacstack=full -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s

; CHECK-LABEL: @func2
; CHECK-NOT: pacia
; CHECK-DAG: eor x30
; CHECK-DAG: eor x30
; CHECK-DAG: eor x30
; CHECK-DAG: eor x30
; CHECK-DAG: mov [[tmpReg:x[0-9]+]], xzr
; CHECK: eor [[tmpReg]]
; CHECK: eor [[tmpReg]]
; CHECK: eor [[tmpReg]]
; CHECK: eor [[tmpReg]]
; CHECK: eor x30, x30, [[tmpReg]]
; CHECK: mov [[tmpReg]], xzr
; CHECK-NOT: pacia
; CHECK: bl
; CHECK-NOT: pacia
; CHECK: mov x30, x28
; CHECK: ld
; CHECK: mov [[tmpReg]], xzr
; CHECK: eor [[tmpReg]]
; CHECK: eor [[tmpReg]]
; CHECK: eor [[tmpReg]]
; CHECK: eor [[tmpReg]]
; CHECK: eor x30
; CHECK: eor x30
; CHECK: eor x30
; CHECK: eor x30
; CHECK-NOT: autia
; CHECK: eor x30, x30, x28
; CHECK: mov [[tmpReg]], xzr
; CHECK: ret
define void @func2() #0 {
  call void @func1()
  ret void
}

declare void @func1()

attributes #0 = { nounwind readnone }
