#include "bus_stops.h"

using namespace std;

Stop::Stop(std::string&& name, Coordinates&& location) :
    name(std::move(name)), location(std::move(location)) {
  assert(name != "");
}

StopPtr StopsManager::AddStop(Stop&& stop) {
  StopPtr result;
  if (auto stop_it = manager.find(stop.name); stop_it != manager.end()) {
    stop_it->second->location = stop.location;
    result = stop_it->second.get();
  } else {
    auto stop_holder = std::make_unique<Stop>(std::move(stop));
    result = stop_holder.get();
    manager[stop_holder->name] = move(stop_holder);
  }
  return result;
}

StopPtr StopsManager::AddStop(std::string&& name, Coordinates&& location) {
  return AddStop(Stop(std::move(name), std::move(location)));
}
