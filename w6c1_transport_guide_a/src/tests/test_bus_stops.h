#pragma once

#include "../bus_stops.h"
#include "test_runner.h"

void TestBusStopsSimple() {
  StopsManager stops;
  ASSERT_EQUAL(stops.manager.size(), 0u);
  ASSERT(!stops.at("Mira"));
  ASSERT_EQUAL(stops.manager.size(), 0u);
  auto mira_ptr = stops["Mira"];
  ASSERT_EQUAL(stops.manager.size(), 1u);
  ASSERT(mira_ptr);
  ASSERT(stops.at("Mira"));
  ASSERT(!stops.at("Mir"));
  ASSERT_EQUAL(mira_ptr, stops.at("Mira"));
  auto another_mira_ptr = stops["Mira"];
  ASSERT_EQUAL(mira_ptr, another_mira_ptr);
  stops.at("Mira")->SetPlace(Coordinates::FromString("56.8532126, 60.6453036"));
  ASSERT_EQUAL(mira_ptr, stops.at("Mira"));
}
