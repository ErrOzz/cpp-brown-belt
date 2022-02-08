#pragma once

#include "parse_line.h"

#include <cmath>
#include <string>
#include <string_view>

class Coordinates {
  double ltt_rad;
  double lgt_rad;

  static constexpr double PI = 3.1415926535;
  static constexpr double EARTH_R = 6'371'000;

  static inline double Rad(double degree) {
    return degree * PI / 180;
  }

  Coordinates(double ltt_deg, double lgt_deg) : ltt_rad(Rad(ltt_deg)),
                                               lgt_rad(Rad(lgt_deg)) {}
public:
   static Coordinates FromString(std::string_view str) {
     const double ltt_deg = ConvertToDouble(ReadToken(str, ", "));
     const double lgt_deg = ConvertToDouble(str);
     return {ltt_deg, lgt_deg};
   }

  double From(const Coordinates& other) const {
    double rad_dist = acos(sin(ltt_rad) * sin(other.ltt_rad) +
                           cos(ltt_rad) * cos(other.ltt_rad) *
                           cos(lgt_rad - other.lgt_rad));
    return rad_dist * EARTH_R;
  }
};

double Distance(const Coordinates& lhs, const Coordinates& rhs) {
  return lhs.From(rhs);
}

struct Stop {
  std::string name;
  Coordinates location;

  bool operator== (const Stop& other) const {
    return name == other.name;
  }
};

struct StopHasher {
  size_t operator() (const Stop& stop) const {
    const std::hash<std::string_view> stop_hasher;
    return stop_hasher(stop.name);
  }
};

//using StopHolder = unique_ptr<Stop>;
//using StopPtr = Stop*;
//
//class StopManager {
//  unordered_set<Stop, StopHasher> stops_set;
//  unordered_map<string_view, StopHolder> stops;
//
//public:
//  const Stop* const GetStop(string_view name) const {
//    return stops.at(name).get();
//  }
//};
