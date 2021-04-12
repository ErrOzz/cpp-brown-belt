#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;
/*
enum class Gender {
  FEMALE,
  MALE
};

struct Person {
  int age;  // возраст
  Gender gender;  // пол
  bool is_employed;  // имеет ли работу
};

// Это пример функции, его не нужно отправлять вместе с функцией PrintStats
template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
  if (range_begin == range_end) {
    return 0;
  }
  vector<typename InputIt::value_type> range_copy(range_begin, range_end);
  auto middle = begin(range_copy) + range_copy.size() / 2;
  nth_element(
      begin(range_copy), middle, end(range_copy),
      [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
      }
  );
  return middle->age;
}
*/
void PrintStats(vector<Person> persons) {
  const auto fem_end = partition(begin(persons), end(persons),
    [](const Person& p) { return p.gender == Gender::FEMALE; });
  const auto emp_fem_end = partition(begin(persons), fem_end,
    [](const Person& p){ return p.is_employed; });
  const auto emp_m_end = partition(fem_end, end(persons),
    [](const Person& p){ return p.is_employed; });
  cout << "Median age = "
       << ComputeMedianAge(begin(persons), end(persons)) << endl;
  cout << "Median age for females = "
       << ComputeMedianAge(begin(persons), fem_end) << endl;
  cout << "Median age for males = "
       << ComputeMedianAge(fem_end, end(persons)) << endl;
  cout << "Median age for employed females = "
       << ComputeMedianAge(begin(persons), emp_fem_end) << endl;
  cout << "Median age for unemployed females = "
       << ComputeMedianAge(emp_fem_end, fem_end) << endl;
  cout << "Median age for employed males = "
       << ComputeMedianAge(fem_end, emp_m_end) << endl;
  cout << "Median age for unemployed males = "
       << ComputeMedianAge(emp_m_end, end(persons)) << endl;
}

int main() {
  vector<Person> persons = {
      {31, Gender::MALE, false},
      {40, Gender::FEMALE, true},
      {24, Gender::MALE, true},
      {20, Gender::FEMALE, true},
      {80, Gender::FEMALE, false},
      {78, Gender::MALE, false},
      {10, Gender::FEMALE, false},
      {55, Gender::MALE, true},
  };
  PrintStats(persons);
  return 0;
}
