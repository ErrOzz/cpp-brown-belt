#pragma once

#include "test_runner.h"


void TestCoordinate() {
  {
  Coordinate a = Coordinate::FromString("55.121314, 56.123456");
  Coordinate b = Coordinate::FromString("55.121314, 56.123456");
  ASSERT_EQUAL(Distance(a, b), 0);
  }
  {
  Coordinate a = Coordinate::FromString("0, 0");
  Coordinate b = Coordinate::FromString("0, 90");
  ASSERT(10007543.3977 <= Distance(a, b) && Distance(a, b) <= 10007543.3978);
  }
  {
  Coordinate a = Coordinate::FromString("0, 0");
  Coordinate b = Coordinate::FromString("90, 0");
  ASSERT(10007543.3977 <= Distance(a, b) && Distance(a, b) <= 10007543.3978);
  }
  {
  Coordinate a = Coordinate::FromString("0, 90");
  Coordinate b = Coordinate::FromString("270, 0");
  ASSERT(10007543.39 <= Distance(a, b) && Distance(a, b) <= 10007543.40);
  }
  {
  Coordinate a = Coordinate::FromString("90, 30");
  Coordinate b = Coordinate::FromString("-90, 30");
  ASSERT(20015086.79 <= Distance(a, b) && Distance(a, b) <= 20015086.80);
  }
  {
  Coordinate a = Coordinate::FromString("0, 0");
  Coordinate b = Coordinate::FromString("0, 180");
  ASSERT(20015086.79 <= Distance(a, b) && Distance(a, b) <= 20015086.80);
  }
  {
  Coordinate Ekaterinburg = Coordinate::FromString("56.83925, 60.60851");
  Coordinate Sidney = Coordinate::FromString("-33.863025, 151.211532");
  ASSERT(Distance(Ekaterinburg, Sidney) >= 13133783.66 &&
         Distance(Ekaterinburg, Sidney) <= 13133783.67);
  }
}


TestRunner tr;

#define TEST(func) RUN_TEST(tr, func)
#define TEST_ALL() {                              \
  TEST(TestCoordinate);                           \
}

