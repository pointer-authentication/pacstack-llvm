; RUN: llc -pacstack=full -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s

; Make sure XR is stashed in X15 before the old one is loaded
;
; CHECK-LABEL: @StopStopwatch_1
; CHECK: bl clock
; CHECK: mov x30, x28
; CHECK: ld{{.*}}x28
; CHECK: pacia [[tmpReg:x[0-9]+]], x28
; CHECK: autia x30, x28
; CHECK: ret
define hidden i64 @StopStopwatch_1(i64 %startticks) local_unnamed_addr #3 {
entry:
  %call = tail call i64 @clock() #10
  %sub = sub i64 %call, %startticks
  ret i64 %sub
}

declare i64 @clock()
declare void @func1()

attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #10 = { nounwind }
