#pragma once

#include "../parse_line.h"
#include "test_runner.h"

void TestSplitTwo() {
  ASSERT_EQUAL(SplitTwo("Final Cut", " ").first, "Final");
  ASSERT_EQUAL(SplitTwo("Final Cut", " ").second, "Cut");
  ASSERT_EQUAL(SplitTwo("Final Cut", "al Cu").first, "Fin");
  ASSERT_EQUAL(SplitTwo("Final Cut", "al Cu").second, "t");
  ASSERT_EQUAL(SplitTwo("    ", " ").first, "");
  ASSERT_EQUAL(SplitTwo("    ", " ").second, "   ");
}

void TestConvertToDouble() {
  ASSERT_EQUAL(ConvertToDouble("3"), 3);
  ASSERT_EQUAL(ConvertToDouble("3.1415926535"), 3.1415926535);
  ASSERT_EQUAL(ConvertToDouble("3.1415926535897932384626433832795"), 3.1415926535897932384626433832795);
}
