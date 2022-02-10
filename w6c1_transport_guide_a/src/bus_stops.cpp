#include "bus_stops.h"

#include <string_view>

using namespace std;

bool Stop::operator==(const Stop& other) const {
  return name == other.name;
}

size_t StopHasher::operator()(const Stop& stop) const {
  const std::hash<std::string_view> stop_hasher;
  return stop_hasher(stop.name);
}

StopsManager::StopsManager() : unordered_set() {};
