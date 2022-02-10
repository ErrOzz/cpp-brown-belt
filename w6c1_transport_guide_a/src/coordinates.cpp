#include "coordinates.h"

#include <cmath>

using namespace std;

inline double Coordinates::Rad(double degree) {
  return degree * PI / 180;
}

Coordinates::Coordinates(double ltt_deg, double lgt_deg) :
                        ltt_rad(Rad(ltt_deg)),
                        lgt_rad(Rad(lgt_deg)) {}

Coordinates Coordinates::FromString(std::string_view str) {
   const double ltt_deg = ConvertToDouble(ReadToken(str, ", "));
   const double lgt_deg = ConvertToDouble(str);
   return {ltt_deg, lgt_deg};
}

double Coordinates::From(const Coordinates& other) const {
  double rad_dist = acos(sin(ltt_rad) * sin(other.ltt_rad) +
                         cos(ltt_rad) * cos(other.ltt_rad) *
                         cos(lgt_rad - other.lgt_rad));
  return rad_dist * EARTH_R;
}

double Distance(const Coordinates& lhs, const Coordinates& rhs) {
  return lhs.From(rhs);
}
