#pragma once

#include "test_bus_stops.h"
#include "test_coordinates.h"
#include "test_parse_line.h"

TestRunner tr;

#define TEST(func) RUN_TEST(tr, func)
#define TEST_ALL() {                              \
  TEST(TestSplitTwo);                             \
  TEST(TestConvertToDouble);                      \
                                                  \
  TEST(TestCoordinates);                          \
                                                  \
  TEST(TestBusStopsSimple);                       \
}
