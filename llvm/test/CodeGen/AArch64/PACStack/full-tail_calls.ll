; RUN: llc -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s
; Make sure tail calls don't confuse the FrameDestroy

; CHECK-LABEL: func2
; CHECK: pacia
; CHECK: pacia
; CHECK: bl func1
; CHECK: pacia
; CHECK: autia
; CHECK: b func1
; CHECK-NOT: ret
define dso_local void @func2() local_unnamed_addr #0 {
  tail call void bitcast (void (...)* @func1 to void ()*)() #2
  tail call void bitcast (void (...)* @func1 to void ()*)() #2
  ret void
}

declare dso_local void @func1(...) local_unnamed_addr #1

attributes #0 = { nounwind "pacstack"="full" "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }
