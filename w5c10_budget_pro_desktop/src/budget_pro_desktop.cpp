#include <iostream>
#include <ctime>
#include <array>

using namespace std;

#define EPOCH_YEAR  1900
#define START_YEAR  2000
#define STOP_YEAR   2099

constexpr size_t DaysTotalCount() {
  size_t days_count = 0;
  for (int year = START_YEAR; year <= STOP_YEAR; ++year) {
    days_count += 365;
    days_count += !(year % 4) && ((year % 100) || !(year % 400)) ? 1 : 0;
  }
  return days_count;
}

class Date {
  int year_;
  int month_;
  int day_;

public:
  Date() = default;
  Date(int year, int month, int day) : year_(year), month_(month), day_(day) {}

  time_t AsTimestamp() const {
    tm t{.tm_mday = day_, .tm_mon = month_ - 1, .tm_year = year_ - EPOCH_YEAR};
    return mktime(&t);
  }

  friend istream& operator>>(istream& input, Date& date);

};

istream& operator>>(istream& input, Date& date) {
  char ignore_c;
  input >> date.year_ >> ignore_c >> date.month_ >> ignore_c >> date.day_;
  return input;
};

class DaySpan {
  size_t begin_ = 0, end_ = 0, qty_ = 0;

  size_t ComputeDay(const Date& date) const {
    const time_t timestamp_from = Date(START_YEAR, 1, 1).AsTimestamp();
    const time_t timestamp_to = date.AsTimestamp();
    constexpr static const int SECONDS_IN_DAY = 60 * 60 * 24;
    return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
  }

public:
  DaySpan(const Date& from, const Date& to) : begin_(ComputeDay(from)),
                                              end_(ComputeDay(to) + 1),
                                              qty_(end_ - begin_) {
  }
  const size_t Begin() const { return begin_; }
  const size_t End() const {return end_; }
  const size_t Qty() const { return qty_; }

};

class IncomeKeeper {
  array<double, DaysTotalCount()> income_;
  array<double, DaysTotalCount()> spends_;

public:
  IncomeKeeper() {
    income_.fill(0);
    spends_.fill(0);
  }

  void Earn(const DaySpan& days, const int value) {
    double val_per_day = static_cast<double>(value) / days.Qty();
    for(size_t id = days.Begin(); id < days.End(); ++id) {
      income_[id] += val_per_day;
    }
  }

  void Spend(const DaySpan& days, const int value) {
    double val_per_day = static_cast<double>(value) / days.Qty();
    for(size_t id = days.Begin(); id < days.End(); ++id) {
      spends_[id] += val_per_day;
    }
  }

  void PayTax(const DaySpan& days, const int percentage) {
    for(size_t id = days.Begin(); id < days.End(); ++ id) {
      income_[id] *= (100 - percentage) / 100.0;
    }
  }

  double ComputeIncome(const DaySpan& days) const {
    double result_income = 0;
    for(size_t id = days.Begin(); id < days.End(); ++ id) {
      result_income += income_[id];
      result_income -= spends_[id];
    }
    return result_income;
  }
};

int main() {
  int q;
  cin >> q;
  IncomeKeeper ik;
  for(int i = 0; i < q; ++i) {
    string query;
    Date from, to;
    cin >> query >> from >> to;
    DaySpan days(from, to);
    cout.precision(25);
    if(query == "Earn") {
        int value;
        cin >> value;
        ik.Earn(days, value);
    } else if(query == "Spend") {
      int value;
      cin >> value;
      ik.Spend(days, value);
    } else if(query == "PayTax") {
        int percentage;
        cin >> percentage;
        ik.PayTax(days, percentage);
    } else if(query == "ComputeIncome") {
        cout << ik.ComputeIncome(days) << '\n';
    }
  }

  return 0;
}
