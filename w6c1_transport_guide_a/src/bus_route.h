#pragma once

#include "bus_stops.h"

#include <string>
#include <vector>

class Route {
  std::string name;
  std::vector<StopPtr> stops;

}
