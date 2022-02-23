#pragma once

#include "parse_line.h"

#include <string_view>

class Coordinates {
  double ltt_rad = 0;
  double lgt_rad = 0;

  static constexpr double PI = 3.1415926535;
  static constexpr double EARTH_R = 6'371'000;

  static inline double Rad(double degree);
  Coordinates(double ltt_deg, double lgt_deg);

public:
  Coordinates() {}
  static Coordinates FromString(std::string_view str);
  double From(const Coordinates& other) const;
};

double Distance(const Coordinates& lhs, const Coordinates& rhs);