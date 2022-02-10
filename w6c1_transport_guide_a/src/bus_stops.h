#pragma once

#include "coordinates.h"

#include <string>
#include <unordered_set>

struct Stop {
  std::string name;
  Coordinates location;

  bool operator==(const Stop& other) const;
};

struct StopHasher {
  size_t operator()(const Stop& stop) const;
};

class StopsManager : public std::unordered_set<Stop, StopHasher> {
  StopsManager();
};
