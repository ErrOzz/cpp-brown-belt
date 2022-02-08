#include "parse_line.h"

#include <optional>
#include <sstream>
#include <string>
#include <charconv>

using namespace std;

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter) {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) return {s, nullopt};
  return {s.substr(0, pos), s.substr(pos + delimiter.length())};
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter) {
  const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter) {
  const auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

#ifdef GCC11
// floating point version works with GCC11?
double ConvertToDouble(string_view str) {
  double result;
  const auto first = str.data(), last = str.data() + str.size();
  auto [ptr, ec] = from_chars(first, last, result);
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
    error << str << " parsed value is not in the range representable by type";
    throw invalid_argument(error.str());
  }
  return result;
}
#else
double ConvertToDouble(string_view str) {
  size_t pos;
  const double result = stod(string(str), &pos);
  if (pos != str.length()) {
    stringstream error;
    error << "string " << str << " contains " << str.length() - pos << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}
#endif
