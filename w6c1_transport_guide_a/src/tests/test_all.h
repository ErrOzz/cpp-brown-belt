#pragma once

#include "test_coordinates.h"
#include "test_parse_line.h"

TestRunner tr;

#define TEST(func) RUN_TEST(tr, func)
#define TEST_ALL() {                              \
  TEST(TestCoordinates);                          \
                                                  \
  TEST(TestSplitTwo);                             \
  TEST(TestConvertToDouble);                      \
}
