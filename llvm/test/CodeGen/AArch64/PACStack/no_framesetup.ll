; RUN: llc -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s
; This checks for a bug whereby the compiler would enter an endless loop.

@arr = global [10 x i32] zeroinitializer

; CHECK-LABEL: @test_func
define i32 @test_func(i32) #0 {
  %2 = icmp slt i32 %0, 100
  br i1 %2, label %3, label %13

3:                                                ; preds = %1
  %4 = sext i32 %0 to i64
  br label %5

5:                                                ; preds = %3, %5
  %6 = phi i64 [ %4, %3 ], [ %11, %5 ]
  %7 = phi i32 [ 1, %3 ], [ %9, %5 ]
  %8 = trunc i64 %6 to i32
  %9 = mul nsw i32 %7, %8
  %10 = getelementptr inbounds [10 x i32], [10 x i32]* @arr, i64 0, i64 %6
  store i32 %9, i32* %10
  %11 = add nsw i64 %6, 10
  %12 = icmp slt i64 %11, 100
  br i1 %12, label %5, label %13

13:                                               ; preds = %5, %1
  %14 = phi i32 [ 1, %1 ], [ %9, %5 ]
  ret i32 %14
}


attributes #0 = { nounwind readnone "pacstack"="full" }
