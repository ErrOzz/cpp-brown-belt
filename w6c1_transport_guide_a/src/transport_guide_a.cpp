#include <iostream>
#include <charconv>
#include <cmath>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

using namespace std;

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) return {s, nullopt};
  return {s.substr(0, pos), s.substr(pos + delimiter.length())};
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter = " ") {
  const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter = " ") {
  const auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

// floating point version works with GCC11
double ConverToType(string_view str) {
  double result;
  const auto first = str.data(), last = str.data() + str.size();
  auto [ptr, ec] = from_chars(first, last, result, chars_format::general);
  if (ec == errc() && ptr != last) {
    stringstream error;
    error << "string " << str << " contains " << last - ptr << " trailing chars";
    throw invalid_argument(error.str());
  } else if (ec == errc::invalid_argument) {
    stringstream error;
    error << "string " << str << " not contains characters that matches the pattern";
    throw invalid_argument(error.str());
  } else if (ec == errc::result_out_of_range) {
    stringstream error;
    error << str << " parsed value is not in the range representable by double type";
    throw invalid_argument(error.str());
  }
  return result;
}

class Coordinate {
  double ltt_rad;
  double lgt_rad;

  static constexpr double PI = 3.1415926535;
  static constexpr double R = 6371000;

  double Rad(double degree) const {
    return PI / 180 * degree;
  }

  Coordinate(double ltt_deg, double lgt_deg) : ltt_rad(Rad(ltt_deg)),
                                               lgt_rad(Rad(lgt_deg)) {}
public:
  static Coordinate FromString(string_view str) {
    const double ltt_deg = ConverToType(ReadToken(str, ", "));
    const double lgt_deg = ConverToType(str);
    return {ltt_deg, lgt_deg};
  }

  double Distance(const Coordinate& other) const {
    double rad_dist = acos(sin(ltt_rad) * sin(other.ltt_rad) +
                           cos(ltt_rad) * cos(other.ltt_rad) *
                           cos(lgt_rad - other.lgt_rad));
    return rad_dist * R;
  }
};

int main() {
  return 0;
}
