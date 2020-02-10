; RUN: llc -O=2 -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s
;
; This just triggers a bug where autia happens before loading x28 from the stack
;

; ModuleID = 'memory.bc'
source_filename = "magick/memory.c"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

@memory_methods.2 = external hidden unnamed_addr global void (i8*)*, align 8

; CHECK-LABEL: @RelinquishMagickMemory
; CHECK: blr
; CHECK: ld{{.*}}x28
; CHECK: autia
; CHECK: ret
; Function Attrs: nounwind
define noalias i8* @RelinquishMagickMemory(i8* %memory) local_unnamed_addr #0 {
entry:
  %cmp = icmp eq i8* %memory, null
  br i1 %cmp, label %return, label %if.end

if.end:                                           ; preds = %entry
  %0 = load void (i8*)*, void (i8*)** @memory_methods.2, align 8, !tbaa !3
  tail call void %0(i8* nonnull %memory) #1
  br label %return

return:                                           ; preds = %entry, %if.end
  ret i8* null
}

attributes #0 = { nounwind "pacstack"="full" "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 9.0.1 (git@version.aalto.fi:platsec/authenticated-shadow-stack/llvm-pafss.git 25e49790b336f0fc834c0c451d6cae7e0c62c6d9)"}
!3 = !{!4, !5, i64 16}
!4 = !{!"_MagickMemoryMethods", !5, i64 0, !5, i64 8, !5, i64 16}
!5 = !{!"any pointer", !6, i64 0}
!6 = !{!"omnipotent char", !7, i64 0}
!7 = !{!"Simple C/C++ TBAA"}
