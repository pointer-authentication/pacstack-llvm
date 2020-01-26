; RUN: llc -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s
; Make sure we store both LR and CR

; CHECK-LABEL: @func2
; CHECK: pacia
; CHECK: eor x30, x30, x15
; CHECK: mov x15, xzr
; CHECK: bl func1
; CHECK: pacia
; CHECK: eor x30, x30, x15
; CHECK: mov x15, xzr
; CHECK: ret
define void @func2() #0 {
  call void @func1()
  ret void
}

declare void @func1()

attributes #0 = { nounwind readnone "pacstack"="full" }
