; RUN: llc -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s
; Make sure we clear the temporary register that holds the mask

; CHECK-LABEL: @regs_saved_x30
; CHECK-DAG: st{{.*}}x30
; CHECK: bl func1
; CHECK-DAG: ld{{.*}}x30
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
