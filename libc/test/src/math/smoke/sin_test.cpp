//===-- Unittests for sin -------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/math/sin.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSinTest = LIBC_NAMESPACE::testing::FPTest<double>;

using LIBC_NAMESPACE::testing::tlog;

TEST_F(LlvmLibcSinTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::sin(sNaN), FE_INVALID);
  EXPECT_MATH_ERRNO(0);

  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::sin(aNaN));
  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::sin(inf));
  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::sin(neg_inf));
  EXPECT_FP_EQ_ALL_ROUNDING(zero, LIBC_NAMESPACE::sin(zero));
  EXPECT_FP_EQ_ALL_ROUNDING(neg_zero, LIBC_NAMESPACE::sin(neg_zero));
  EXPECT_FP_EQ(0x1.0p-50, LIBC_NAMESPACE::sin(0x1.0p-50));
  EXPECT_FP_EQ(min_normal, LIBC_NAMESPACE::sin(min_normal));
  EXPECT_FP_EQ(min_denormal, LIBC_NAMESPACE::sin(min_denormal));
}

#ifdef LIBC_TEST_FTZ_DAZ

using namespace LIBC_NAMESPACE::testing;

TEST_F(LlvmLibcSinTest, FTZMode) {
  ModifyMXCSR mxcsr(FTZ);

  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::sin(min_denormal));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::sin(max_denormal));
}

TEST_F(LlvmLibcSinTest, DAZMode) {
  ModifyMXCSR mxcsr(DAZ);

  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::sin(min_denormal));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::sin(max_denormal));
}

TEST_F(LlvmLibcSinTest, FTZDAZMode) {
  ModifyMXCSR mxcsr(FTZ | DAZ);

  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::sin(min_denormal));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::sin(max_denormal));
}

#endif
