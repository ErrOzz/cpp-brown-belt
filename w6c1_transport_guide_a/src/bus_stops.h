#pragma once

#include "coordinates.h"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class Stop {
  std::string name;
  Coordinates location;

public:
  Stop(const Stop& other) = delete;
  Stop(Stop&& other) = default;
  Stop(std::string&& name, Coordinates&& location = {});

  Stop& operator=(const Stop& other) = delete;
  Stop& operator=(Stop&& other) = default;

  friend class StopsManager;
};

using StopHolder = std::unique_ptr<Stop>;
using StopPtr = Stop*;

class StopsManager {
  std::unordered_map<std::string_view, StopHolder> manager;

public:
  StopPtr AddStop(Stop&& stop);
  StopPtr AddStop(std::string&& name, Coordinates&& location = {});
  
};
