; RUN: llc -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -verify-machineinstrs < %s | FileCheck %s
; This checks for a bug whereby the compiler would enter an endless loop.

@arr = global [10 x i64] zeroinitializer

; CHECK-LABEL: @test_func
define i32 @test_func(i32) #0 {
  %2 = icmp slt i32 %0, 100
  br i1 %2, label %3, label %16

3:                                                ; preds = %1
  %4 = sext i32 %0 to i64
  br label %5

5:                                                ; preds = %3, %5
  %6 = phi i64 [ %4, %3 ], [ %12, %5 ]
  %7 = phi i32 [ 1, %3 ], [ %9, %5 ]
  %8 = trunc i64 %6 to i32
  %9 = mul nsw i32 %7, %8
  %10 = sext i32 %9 to i64
  %11 = getelementptr inbounds [10 x i64], [10 x i64]* @arr, i64 0, i64 %6
  store i64 %10, i64* %11
  %12 = add nsw i64 %6, 10
  %13 = icmp slt i64 %12, 100
  br i1 %13, label %5, label %14

14:                                               ; preds = %5
  %15 = trunc i64 %12 to i32
  br label %16

16:                                               ; preds = %14, %1
  %17 = phi i32 [ %0, %1 ], [ %15, %14 ]
  %18 = phi i32 [ 1, %1 ], [ %9, %14 ]
  %19 = and i32 %17, 1
  %20 = icmp eq i32 %19, 0
  br i1 %20, label %21, label %23

21:                                               ; preds = %16
  tail call void asm sideeffect "nop", "~{x0},~{x1},~{x2},~{x3},~{x4},~{x5},~{x6},~{x7},~{x8},~{x9},~{x10},~{x11},~{x12},~{x13},~{x14},~{x15},~{x16},~{x17},~{x18},~{x19},~{x20},~{x21},~{x22},~{x23},~{x24},~{x25},~{x26},~{x27}"()
  %22 = sext i32 %17 to i64
  store i64 %22, i64* getelementptr inbounds ([10 x i64], [10 x i64]* @arr, i64 0, i64 0)
  br label %23

23:                                               ; preds = %21, %16
  ret i32 %18
}



attributes #0 = { nounwind readnone "pacstack"="full" }
