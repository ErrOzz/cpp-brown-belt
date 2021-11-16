#include <iostream>
#include <sstream>
#include <iterator>
#include <string>
#include <optional>
#include <charconv>
#include <ctime>
#include <memory>
#include <vector>
#include <unordered_map>
#include <numeric>

using namespace std;

template <typename It>
class Range {
  It begin_, end_;

public:
  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }
  size_t Size() { return distance(begin_, end_); }
};

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
  size_t pos = s.find(delimiter);
  if (pos == s.npos) { return {s, nullopt}; }
  return {s.substr(0, pos), s.substr(pos + delimiter.size())};
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter = " ") {
  auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter = " ") {
  auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

int ConvertToInt(string_view str) {
  size_t result;
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
    error << str << " parsed value is not in the range representable by int type";
    throw invalid_argument(error.str());
  }
  return result;
}

template <typename Number>
void ValidateBounds(Number to_validate, Number min_value, Number max_value) {
  if (min_value > to_validate || to_validate > max_value) {
    stringstream error;
    error << to_validate << " is out of [" << min_value << ", " << max_value <<"]";
    throw out_of_range(error.str());
  }
}

struct MoneyState {
  double earned = 0.0;
  double spent = 0.0;

  double ComputeIncome() const {
    return earned - spent;
  }

  MoneyState& operator+=(const MoneyState& other) {
    earned += other.earned;
    spent += other.spent;
    return *this;
  }

  MoneyState operator+(const MoneyState& other) const {
    return MoneyState(*this) += other;
  }

  MoneyState operator*(double factor) const {
    return {earned * factor, spent * factor};
  }
};

ostream& operator<<(ostream& stream, const MoneyState& state) {
  return stream << "{+" << state.earned << ", -" << state.spent << "}";
}

struct IndexSegment {
  size_t first;
  size_t last;

  size_t Length() {
    return last - first;
  }

  bool Empty() {
    return Length() == 0;
  }

  bool Contains(const IndexSegment& other) {
    return first <= other.first && other.last <= last;
  }
};

class Date {
  int year_, month_, day_;

  Date(int year, int month, int day) : year_(year), month_(month), day_(day) {}

public:
  static Date FromString(string_view str) {
    const int year = ConvertToInt(ReadToken(str, "-"));
    const int month = ConvertToInt(ReadToken(str, "-"));
    ValidateBounds(month, 1, 12);
    const int day = ConvertToInt(str);
    ValidateBounds(day, 1, 31);
    return {year, month, day};
  }

  time_t AsTimestamp() const {
    tm t{.tm_mday = day_, .tm_mon = month_ - 1, .tm_year = year_ - 1900};
    return mktime(&t);
  }
};

int ComputeDaysDiff(const Date& date_from, const Date& date_to) {
  const time_t timestamp_from = date_from.AsTimestamp();
  const time_t timestamp_to = date_to.AsTimestamp();
  static constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
  return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

static const Date START_DATE = Date::FromString("2000-01-01");
static const Date STOP_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = ComputeDaysDiff(START_DATE, STOP_DATE);

size_t ComputeDayIndex(const Date& date) {
  return ComputeDaysDiff(START_DATE, date);
}

IndexSegment MakeDateSegment(const Date& date_from, const Date& date_to) {
  return {ComputeDayIndex(date_from), ComputeDayIndex(date_to)};
}

class BudgetManager : public vector<MoneyState> {
public:
  BudgetManager() : vector(DAY_COUNT) {}
  auto MakeDateRange(const Date& date_from, const Date& date_to) const {
    const auto segment = MakeDateSegment(date_from, date_to);
    return Range(begin() + segment.first, begin() + segment.last);
  }
  auto MakeDateRange(const Date& date_from, const Date& date_to) {
    const auto segment = MakeDateSegment(date_from, date_to);
    return Range(begin() + segment.first, begin() + segment.last);
  }
};


struct Request;
using RequestHolder = unique_ptr<Request>;

struct Request {
  enum class Type {
    COMPUTE_INCOME,
    EARN,
    SPEND,
    PAY_TAX
  };

  const Type type;

  Request(Type type) : type(type) {}
  static RequestHolder Create(Type type);
  virtual void ParseFrom(string_view input) = 0;
  virtual ~Request() = default;
};

const unordered_map<string_view, Request::Type> STR_TO_REQUEST_TYPE = {
    {"ComputeIncome", Request::Type::COMPUTE_INCOME},
    {"Earn", Request::Type::EARN},
    {"Spend", Request::Type::SPEND},
    {"PayTax", Request::Type::PAY_TAX}
};

template <typename ResultType>
struct ReadRequest : Request {
  using Request::Request;
  virtual ResultType Process(const BudgetManager& manager) const = 0;
};

struct ModifyRequest : Request {
  using Request::Request;
  virtual void Process(const BudgetManager& manager) const = 0;
};

struct ComputeIncomeRequest : ReadRequest<double> {
  Date date_from = START_DATE;
  Date date_to = START_DATE;

  ComputeIncomeRequest() : ReadRequest(Type::COMPUTE_INCOME) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(input);
  }

  double Process(const BudgetManager& manager) const override {
    const auto range = manager.MakeDateRange(date_from, date_to);
    return accumulate(range.begin(), range.end(), MoneyState{}).ComputeIncome();
  }
};

int main () {
  return 0;
}
