; RUN: llc -O=1 -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s
; This just triggers a bug encountered in nbench, doesn't check more than that
;
; CHECK-LABEL: @show_stats

; ModuleID = 'nbench0.bc'
source_filename = "nbench0.c"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

%struct.SortStruct = type { i32, i64, double, i16, i64 }
%struct.BitOpStruct = type { i32, i64, double, i64, i64 }
%struct.EmFloatStruct = type { i32, i64, i64, i64, double }
%struct.FourierStruct = type { i32, i64, i64, double }
%struct.AssignStruct = type { i32, i64, i64, double }
%struct.IDEAStruct = type { i32, i64, i64, i64, double }
%struct.HuffStruct = type { i32, i64, i64, i64, double }
%struct.NNetStruct = type { i32, i64, i64, double }
%struct.LUStruct = type { i32, i64, i64, double }

@global_numsortstruct = external hidden global %struct.SortStruct, align 8
@global_strsortstruct = external hidden global %struct.SortStruct, align 8
@global_bitopstruct = external hidden global %struct.BitOpStruct, align 8
@global_emfloatstruct = external hidden global %struct.EmFloatStruct, align 8
@global_fourierstruct = external hidden global %struct.FourierStruct, align 8
@global_assignstruct = external hidden global %struct.AssignStruct, align 8
@global_ideastruct = external hidden global %struct.IDEAStruct, align 8
@global_huffstruct = external hidden global %struct.HuffStruct, align 8
@global_nnetstruct = external hidden global %struct.NNetStruct, align 8
@global_lustruct = external hidden global %struct.LUStruct, align 8
@.str.102 = external hidden unnamed_addr constant [24 x i8], align 1
@.str.103 = external hidden unnamed_addr constant [19 x i8], align 1
@.str.104 = external hidden unnamed_addr constant [30 x i8], align 1
@.str.105 = external hidden unnamed_addr constant [28 x i8], align 1
@.str.106 = external hidden unnamed_addr constant [24 x i8], align 1
@.str.107 = external hidden unnamed_addr constant [19 x i8], align 1
@.str.108 = external hidden unnamed_addr constant [31 x i8], align 1
@.str.109 = external hidden unnamed_addr constant [25 x i8], align 1
@.str.110 = external hidden unnamed_addr constant [23 x i8], align 1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #0

; Function Attrs: nofree nounwind
declare hidden fastcc void @output_string(i8*) unnamed_addr #1

; Function Attrs: nofree nounwind
declare i32 @sprintf(i8* nocapture, i8* nocapture readonly, ...) local_unnamed_addr #2

; Function Attrs: nounwind
define hidden fastcc void @show_stats(i32 %bid) unnamed_addr #3 {
entry:
  %buffer = alloca [80 x i8], align 1
  %0 = getelementptr inbounds [80 x i8], [80 x i8]* %buffer, i64 0, i64 0
  call void @llvm.lifetime.start.p0i8(i64 80, i8* nonnull %0) #4
  switch i32 %bid, label %sw.epilog [
    i32 0, label %sw.bb
    i32 1, label %sw.bb5
    i32 2, label %sw.bb13
    i32 3, label %sw.bb20
    i32 4, label %sw.epilog.sink.split
    i32 5, label %sw.bb31
    i32 6, label %sw.bb35
    i32 7, label %sw.bb42
    i32 8, label %sw.bb49
    i32 9, label %sw.bb53
  ]

sw.bb:                                            ; preds = %entry
  %1 = load i16, i16* getelementptr inbounds (%struct.SortStruct, %struct.SortStruct* @global_numsortstruct, i64 0, i32 3), align 8, !tbaa !3
  %conv = zext i16 %1 to i32
  %call = call i32 (i8*, i8*, ...) @sprintf(i8* nonnull %0, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.102, i64 0, i64 0), i32 %conv) #4
  call fastcc void @output_string(i8* nonnull %0)
  br label %sw.epilog.sink.split

sw.bb5:                                           ; preds = %entry
  %2 = load i16, i16* getelementptr inbounds (%struct.SortStruct, %struct.SortStruct* @global_strsortstruct, i64 0, i32 3), align 8, !tbaa !3
  %conv7 = zext i16 %2 to i32
  %call8 = call i32 (i8*, i8*, ...) @sprintf(i8* nonnull %0, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.102, i64 0, i64 0), i32 %conv7) #4
  call fastcc void @output_string(i8* nonnull %0)
  br label %sw.epilog.sink.split

sw.bb13:                                          ; preds = %entry
  %3 = load i64, i64* getelementptr inbounds (%struct.BitOpStruct, %struct.BitOpStruct* @global_bitopstruct, i64 0, i32 3), align 8, !tbaa !11
  %call15 = call i32 (i8*, i8*, ...) @sprintf(i8* nonnull %0, i8* getelementptr inbounds ([30 x i8], [30 x i8]* @.str.104, i64 0, i64 0), i64 %3) #4
  call fastcc void @output_string(i8* nonnull %0)
  br label %sw.epilog.sink.split

sw.bb20:                                          ; preds = %entry
  %4 = load i64, i64* getelementptr inbounds (%struct.EmFloatStruct, %struct.EmFloatStruct* @global_emfloatstruct, i64 0, i32 3), align 8, !tbaa !13
  %call22 = call i32 (i8*, i8*, ...) @sprintf(i8* nonnull %0, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.106, i64 0, i64 0), i64 %4) #4
  call fastcc void @output_string(i8* nonnull %0)
  br label %sw.epilog.sink.split

sw.bb31:                                          ; preds = %entry
  br label %sw.epilog.sink.split

sw.bb35:                                          ; preds = %entry
  %5 = load i64, i64* getelementptr inbounds (%struct.IDEAStruct, %struct.IDEAStruct* @global_ideastruct, i64 0, i32 2), align 8, !tbaa !15
  %call37 = call i32 (i8*, i8*, ...) @sprintf(i8* nonnull %0, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.107, i64 0, i64 0), i64 %5) #4
  call fastcc void @output_string(i8* nonnull %0)
  br label %sw.epilog.sink.split

sw.bb42:                                          ; preds = %entry
  %6 = load i64, i64* getelementptr inbounds (%struct.HuffStruct, %struct.HuffStruct* @global_huffstruct, i64 0, i32 2), align 8, !tbaa !15
  %call44 = call i32 (i8*, i8*, ...) @sprintf(i8* nonnull %0, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.107, i64 0, i64 0), i64 %6) #4
  call fastcc void @output_string(i8* nonnull %0)
  br label %sw.epilog.sink.split

sw.bb49:                                          ; preds = %entry
  br label %sw.epilog.sink.split

sw.bb53:                                          ; preds = %entry
  br label %sw.epilog.sink.split

sw.epilog.sink.split:                             ; preds = %entry, %sw.bb, %sw.bb5, %sw.bb13, %sw.bb20, %sw.bb31, %sw.bb35, %sw.bb42, %sw.bb49, %sw.bb53
  %.sink58 = phi i64* [ getelementptr inbounds (%struct.LUStruct, %struct.LUStruct* @global_lustruct, i64 0, i32 2), %sw.bb53 ], [ getelementptr inbounds (%struct.NNetStruct, %struct.NNetStruct* @global_nnetstruct, i64 0, i32 2), %sw.bb49 ], [ getelementptr inbounds (%struct.HuffStruct, %struct.HuffStruct* @global_huffstruct, i64 0, i32 3), %sw.bb42 ], [ getelementptr inbounds (%struct.IDEAStruct, %struct.IDEAStruct* @global_ideastruct, i64 0, i32 3), %sw.bb35 ], [ getelementptr inbounds (%struct.AssignStruct, %struct.AssignStruct* @global_assignstruct, i64 0, i32 2), %sw.bb31 ], [ getelementptr inbounds (%struct.EmFloatStruct, %struct.EmFloatStruct* @global_emfloatstruct, i64 0, i32 2), %sw.bb20 ], [ getelementptr inbounds (%struct.BitOpStruct, %struct.BitOpStruct* @global_bitopstruct, i64 0, i32 4), %sw.bb13 ], [ getelementptr inbounds (%struct.SortStruct, %struct.SortStruct* @global_strsortstruct, i64 0, i32 4), %sw.bb5 ], [ getelementptr inbounds (%struct.SortStruct, %struct.SortStruct* @global_numsortstruct, i64 0, i32 4), %sw.bb ], [ getelementptr inbounds (%struct.FourierStruct, %struct.FourierStruct* @global_fourierstruct, i64 0, i32 2), %entry ]
  %.sink = phi i8* [ getelementptr inbounds ([25 x i8], [25 x i8]* @.str.109, i64 0, i64 0), %sw.bb53 ], [ getelementptr inbounds ([24 x i8], [24 x i8]* @.str.106, i64 0, i64 0), %sw.bb49 ], [ getelementptr inbounds ([24 x i8], [24 x i8]* @.str.106, i64 0, i64 0), %sw.bb42 ], [ getelementptr inbounds ([23 x i8], [23 x i8]* @.str.110, i64 0, i64 0), %sw.bb35 ], [ getelementptr inbounds ([25 x i8], [25 x i8]* @.str.109, i64 0, i64 0), %sw.bb31 ], [ getelementptr inbounds ([19 x i8], [19 x i8]* @.str.107, i64 0, i64 0), %sw.bb20 ], [ getelementptr inbounds ([28 x i8], [28 x i8]* @.str.105, i64 0, i64 0), %sw.bb13 ], [ getelementptr inbounds ([19 x i8], [19 x i8]* @.str.103, i64 0, i64 0), %sw.bb5 ], [ getelementptr inbounds ([19 x i8], [19 x i8]* @.str.103, i64 0, i64 0), %sw.bb ], [ getelementptr inbounds ([31 x i8], [31 x i8]* @.str.108, i64 0, i64 0), %entry ]
  %7 = load i64, i64* %.sink58, align 8, !tbaa !16
  %call55 = call i32 (i8*, i8*, ...) @sprintf(i8* nonnull %0, i8* %.sink, i64 %7) #4
  call fastcc void @output_string(i8* nonnull %0)
  br label %sw.epilog

sw.epilog:                                        ; preds = %sw.epilog.sink.split, %entry
  call void @llvm.lifetime.end.p0i8(i64 80, i8* nonnull %0) #4
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #0

attributes #0 = { argmemonly nounwind }
attributes #1 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "pacstack"="full" "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 9.0.1 (PACStack 32357e3940eb5b0ba30be8a2da14d04272fb40cb)"}
!3 = !{!4, !10, i64 24}
!4 = !{!"", !5, i64 0, !8, i64 8, !9, i64 16, !10, i64 24, !8, i64 32}
!5 = !{!"int", !6, i64 0}
!6 = !{!"omnipotent char", !7, i64 0}
!7 = !{!"Simple C/C++ TBAA"}
!8 = !{!"long", !6, i64 0}
!9 = !{!"double", !6, i64 0}
!10 = !{!"short", !6, i64 0}
!11 = !{!12, !8, i64 24}
!12 = !{!"", !5, i64 0, !8, i64 8, !9, i64 16, !8, i64 24, !8, i64 32}
!13 = !{!14, !8, i64 24}
!14 = !{!"", !5, i64 0, !8, i64 8, !8, i64 16, !8, i64 24, !9, i64 32}
!15 = !{!14, !8, i64 16}
!16 = !{!8, !8, i64 0}