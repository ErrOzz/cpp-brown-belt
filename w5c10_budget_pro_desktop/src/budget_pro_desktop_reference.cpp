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
  size_t size() const { return distance(begin_, end_); }
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
  return {ComputeDayIndex(date_from), ComputeDayIndex(date_to) + 1};
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
  virtual void Process(BudgetManager& manager) const = 0;
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

template <int SIGN>
struct AddMoneyRequest : ModifyRequest {
  static_assert(SIGN == -1 || SIGN == 1);

  Date date_from = START_DATE;
  Date date_to = START_DATE;
  size_t value = 0;

  AddMoneyRequest() : ModifyRequest(SIGN == 1 ? Type::EARN : Type::SPEND) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    value = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    const auto range = manager.MakeDateRange(date_from, date_to);
    const double daily_value = value * 1.0 / size(range);
    const MoneyState daily_change = SIGN == 1 ?
                                    MoneyState{.earned = daily_value} :
                                    MoneyState{.spent = daily_value};
    for (auto& money : range) {
      money += daily_change;
    }
  }
};

struct PayTax : ModifyRequest {
  Date date_from = START_DATE;
  Date date_to = START_DATE;
  uint8_t percentage = 0;

  PayTax() : ModifyRequest(Type::PAY_TAX) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    percentage = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    for (auto& money : manager.MakeDateRange(date_from, date_to)) {
      money.earned *= 1 - percentage / 100.0;
    }
  }
};

RequestHolder Request::Create(Request::Type type) {
  switch (type) {
    case Request::Type::COMPUTE_INCOME:
      return make_unique<ComputeIncomeRequest>();
    case Request::Type::EARN:
      return make_unique<AddMoneyRequest<+1>>();
    case Request::Type::SPEND:
      return make_unique<AddMoneyRequest<-1>>();
    case Request::Type::PAY_TAX:
      return make_unique<PayTax>();
    default:
      return nullptr;
  }
}

template <typename Number>
Number ReadNumberOnLine(istream& stream) {
  Number number;
  stream >> number;
  string dummy;
  getline(stream, dummy);
  return number;
}

optional<Request::Type> ConvertRequetTypeFromString(string_view type_str) {
  if (const auto it = STR_TO_REQUEST_TYPE.find(type_str);
      it != STR_TO_REQUEST_TYPE.end()) {
    return it->second;
  } else {
    return nullopt;
  }
}

RequestHolder ParseRequest(string_view request_str) {
  const auto request_type = ConvertRequetTypeFromString(ReadToken(request_str));
  if (!request_type) return nullptr;
  RequestHolder request = Request::Create(*request_type);
  if (request) request->ParseFrom(request_str);
  return request;
}

vector<RequestHolder> ReadRequests(istream& in_stream = cin) {
  const size_t request_count = ReadNumberOnLine<size_t>(in_stream);
  vector<RequestHolder> requests;
  requests.reserve(request_count);
  for (size_t i = 0; i < request_count; ++i) {
    string request_str;
    getline(in_stream, request_str);
    if (auto request = ParseRequest(request_str)) {
      requests.push_back(move(request));
    }
  }
  return requests;
}

vector<double> ProcessRequests(const vector<RequestHolder>& requests) {
  vector<double> responses;
  BudgetManager manager;
  for (const auto& request_holder : requests) {
    if (request_holder->type == Request::Type::COMPUTE_INCOME) {
      const auto& request = static_cast<const ComputeIncomeRequest&>(*request_holder);
      responses.push_back(request.Process(manager));
    } else {
      const auto& request = static_cast<const ModifyRequest&>(*request_holder);
      request.Process(manager);
    }
  }
  return responses;
}

void PrintResponses(const vector<double>& responses, ostream& stream = cout) {
  for (const double response : responses) {
    stream << response << endl;
  }
}

int main () {
  cout.precision(25);
  const auto requests = ReadRequests();
  const auto responses = ProcessRequests(requests);
  PrintResponses(responses);
  return 0;
}
