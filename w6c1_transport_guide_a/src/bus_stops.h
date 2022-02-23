#pragma once

#include "coordinates.h"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class Stop;
using StopPtr = Stop*;
using StopHolder = std::unique_ptr<Stop>;

class Stop {
  std::string name;
  Coordinates location;

  Stop(const Stop& other) = delete;
  Stop(std::string&& name);
  Stop& operator=(const Stop& other) = delete;

  friend class StopsManager;
public:
  Stop(Stop&& other) = default;

  StopPtr SetPlace(Coordinates&& location);
};

class StopsManager {
  std::unordered_map<std::string_view, StopHolder> manager;

public:
  StopPtr at(const std::string& name);
  StopPtr operator[](std::string&& name);

#ifdef TESTS
  friend void TestBusStopsSimple();
#endif
};
