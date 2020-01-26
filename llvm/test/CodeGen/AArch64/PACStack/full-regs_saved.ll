; RUN: llc -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s
; XFAIL: *
; Make sure we clear the temporary register that holds the mask

; CHECK-LABEL: @func2
; CHECK-DAG: st{{.*}}x28
; CHECK-DAG: st{{.*}}x30
; CHECK: bl func1
; CHECK-DAG: ld{{.*}}x28
; CHECK-DAG: ld{{.*}}x30
; CHECK: ret
define void @func2() #0 {
  call void @func1()
  ret void
}

declare void @func1()

attributes #0 = { nounwind readnone "pacstack"="full" }
