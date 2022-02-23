#include "bus_stops.h"

using namespace std;

Stop::Stop(string&& name) :
    name(move(name)), location({}) {
  assert(name == "");
}

StopPtr Stop::SetPlace(Coordinates&& location) {
  this->location = move(location);
  return this;
}

StopPtr StopsManager::at(const std::string& name) {
  if (auto stop_it = manager.find(name); stop_it != manager.end()) {
    return stop_it->second.get();
  } else {
    return nullptr;
  }
}

StopPtr StopsManager::operator[](std::string&& name) {
  if (auto stop_ptr = at(name)) {
    return stop_ptr;
  }
  auto stop_holder = make_unique<Stop>(Stop(move(name)));
  return manager.emplace(stop_holder->name, move(stop_holder)).first->second.get();
}
