#include <iostream>
#include <ctime>
#include <array>

using namespace std;

#define START_YEAR  2000
#define STOP_YEAR   2099
#define TAX         0.87

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
    tm t{.tm_mday = day_, .tm_mon = month_ - 1, .tm_year = year_ - 1900};
    return mktime(&t);
  }

  friend istream& operator>>(istream& input, Date& date);

};

istream& operator>>(istream& input, Date& date) {
  char ignore_c;
  input >> date.year_ >> ignore_c >> date.month_ >> ignore_c >> date.day_;
  return input;
};

class IncomeKeeper {
  array<double, DaysTotalCount()> income_;

  size_t ComputeId(const Date& date) const {
    const time_t timestamp_from = Date{START_YEAR, 1, 1}.AsTimestamp();
    const time_t timestamp_to = date.AsTimestamp();
    constexpr static const int SECONDS_IN_DAY = 60 * 60 * 24;
    return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
  }

public:
  IncomeKeeper() {
    income_.fill(0);
  }

  void Earn(const Date& from, const Date& to, const int value) {
    const size_t id_from = ComputeId(from), id_to = ComputeId(to);
    const size_t days = id_to - id_from + 1;
    double val_per_day = static_cast<double>(value) / days;
    for(size_t id = ComputeId(from); id <= id_to; ++id) {
      income_[id] += val_per_day;
    }
  }

  void PayTax(const Date& from, const Date& to) {
    const size_t id_to = ComputeId(to);
    for(size_t id = ComputeId(from); id <= id_to; ++ id) {
      income_[id] *= TAX;
    }
  }

  double ComputeIncome(const Date& from, const Date& to) const {
    const size_t id_to = ComputeId(to);
    double result_income = 0;
    for(size_t id = ComputeId(from); id <= id_to; ++ id) {
      result_income += income_[id];
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
    cout.precision(25);
    if(query == "Earn") {
        int value;
        cin >> value;
        ik.Earn(from, to, value);
    } else if(query == "PayTax") {
        ik.PayTax(from, to);
    } else if(query == "ComputeIncome") {
        cout << ik.ComputeIncome(from, to) << '\n';
    }
  }

  return 0;
}
