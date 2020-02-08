; RUN: llc -O=0 -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s

; CHECK-LABEL: @func2
; CHECK: pacia
; CHECK: pacia
; CHECK: bl
; CHECK: pacia
; CHECK: autia
; CHECK: ret
define void @func2() #0 {
  call void @func1()
  ret void
}

; CHECK-LABEL: @func3
; CHECK: pacia
; CHECK: eor x30, x30, x15
; CHECK: mov x15, xzr
; CHECK: bl func1
; CHECK: pacia
; CHECK: eor x30, x30, x15
; CHECK: mov x15, xzr
; CHECK: ret
define void @func3() #0 {
  call void @func1()
  ret void
}

; CHECK-LABEL: @regs_saved_x30
; CHECK-DAG: st{{.*}}x30
; CHECK: bl func1
; CHECK: ret
define void @regs_saved_x30() #0 {
  call void @func1()
  ret void
}

; CHECK-LABEL: @regs_saved_x28
; CHECK-DAG: st{{.*}}x28
; CHECK: bl func1
; CHECK-DAG: ld{{.*}}x28
; CHECK: ret
define void @regs_saved_x28() #0 {
  call void @func1()
  ret void
}

declare void @func1()

attributes #0 = { nounwind readnone "pacstack"="full" }
