; RUN: llc -pacstack=full -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s

; CHECK-LABEL: @func2
; CHECK-DAG: pacia  x30, x28
; CHECK-DAG: mov [[tmpReg:x[0-9]+]], xzr
; CHECK: pacia [[tmpReg]], x28
; CHECK: eor x30, x30, [[tmpReg]]
; CHECK-DAG: mov [[tmpReg]], xzr
; CHECK-DAG: mov x28, x30
; CHECK: bl
; CHECK: mov [[tmpReg]], xzr
; CHECK: pacia [[tmpReg]], x28
; CHECK: eor x30, x30, [[tmpReg]]
; CHECK-DAG: mov [[tmpReg]], xzr
; CHECK-DAG: autia x30, x28
; CHECK: ret
define void @func2() #0 {
  call void @func1()
  ret void
}

declare void @func1()

attributes #0 = { nounwind readnone }
