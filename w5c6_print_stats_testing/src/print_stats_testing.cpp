#define HOME_TESTING
#ifdef HOME_TESTING

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class Gender {
  FEMALE,
  MALE
};

struct Person {
  int age;
  Gender gender;
  bool is_employed;
};

bool operator==(const Person& lhs, const Person& rhs) {
  return lhs.age == rhs.age
      && lhs.gender == rhs.gender
      && lhs.is_employed == rhs.is_employed;
}

ostream& operator<<(ostream& stream, const Person& person) {
  return stream << "Person(age=" << person.age
      << ", gender=" << static_cast<int>(person.gender)
      << ", is_employed=" << boolalpha << person.is_employed << ")";
}

struct AgeStats {
  int total;
  int females;
  int males;
  int employed_females;
  int unemployed_females;
  int employed_males;
  int unemployed_males;
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
  if (range_begin == range_end) {
    return 0;
  }
  vector<typename iterator_traits<InputIt>::value_type> range_copy(
      range_begin,
      range_end
  );
  auto middle = begin(range_copy) + range_copy.size() / 2;
  nth_element(
      begin(range_copy), middle, end(range_copy),
      [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
      }
  );
  return middle->age;
}

vector<Person> ReadPersons(istream& in_stream = cin) {
  int person_count;
  in_stream >> person_count;
  vector<Person> persons;
  persons.reserve(person_count);
  for (int i = 0; i < person_count; ++i) {
    int age, gender, is_employed;
    in_stream >> age >> gender >> is_employed;
    Person person{
        age,
        static_cast<Gender>(gender),
        is_employed == 1
    };
    persons.push_back(person);
  }
  return persons;
}

AgeStats ComputeStats(vector<Person> persons) {
  //                 persons
  //                |       |
  //          females        males
  //         |       |      |     |
  //      empl.  unempl. empl.   unempl.

  auto females_end = partition(
      begin(persons), end(persons),
      [](const Person& p) {
        return p.gender == Gender::FEMALE;
      }
  );
  auto employed_females_end = partition(
      begin(persons), females_end,
      [](const Person& p) {
        return p.is_employed;
      }
  );
  auto employed_males_end = partition(
      females_end, end(persons),
      [](const Person& p) {
        return p.is_employed;
      }
  );

  return {
       ComputeMedianAge(begin(persons), end(persons)),
       ComputeMedianAge(begin(persons), females_end),
       ComputeMedianAge(females_end, end(persons)),
       ComputeMedianAge(begin(persons),  employed_females_end),
       ComputeMedianAge(employed_females_end, females_end),
       ComputeMedianAge(females_end, employed_males_end),
       ComputeMedianAge(employed_males_end, end(persons))
  };
}

void PrintStats(const AgeStats& stats,
                ostream& out_stream = cout) {
  out_stream << "Median age = "
             << stats.total              << endl
             << "Median age for females = "
             << stats.females            << endl
             << "Median age for males = "
             << stats.males              << endl
             << "Median age for employed females = "
             << stats.employed_females   << endl
             << "Median age for unemployed females = "
             << stats.unemployed_females << endl
             << "Median age for employed males = "
             << stats.employed_males     << endl
             << "Median age for unemployed males = "
             << stats.unemployed_males   << endl;
}

#endif

#include "test_runner.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void TestComputeMedianAgeVoidRange() {
  vector<Person> persons;
  ASSERT(!ComputeMedianAge(begin(persons), end(persons)));
}

void TestComputeMedianAgeOddSizeRange() {
  vector<Person> persons {
    {1, Gender::FEMALE, false},
    {2, Gender::FEMALE, false},
    {3, Gender::FEMALE, false},
    {4, Gender::FEMALE, false},
    {5, Gender::FEMALE, false}
  };
  ASSERT_EQUAL(ComputeMedianAge(begin(persons), end(persons)), 3);
}

void TestReadPersons() {
  string s = "2 10 1 0 20 0 1";
  istringstream ss(s);
  vector<Person> persons = ReadPersons(ss);
  ASSERT_EQUAL(persons.size(), 2u);
  ASSERT_EQUAL(persons[0].age, 10);
  ASSERT(persons[0].gender == Gender::MALE);
  ASSERT_EQUAL(persons[0].is_employed, false);
  ASSERT_EQUAL(persons[1].age, 20);
  ASSERT(persons[1].gender == Gender::FEMALE);
  ASSERT_EQUAL(persons[1].is_employed, true);
}

void TestComputeStats() {
  vector<Person> persons {
    {1, Gender::FEMALE, true},
    {2, Gender::FEMALE, false},
    {3, Gender::MALE, true},
    {4, Gender::MALE, true},
    {5, Gender::MALE, false}
  };
  AgeStats stats = ComputeStats(persons);
  ASSERT_EQUAL(stats.total, 3);
  ASSERT_EQUAL(stats.females, 2);
  ASSERT_EQUAL(stats.males, 4);
  ASSERT_EQUAL(stats.employed_females, 1);
  ASSERT_EQUAL(stats.unemployed_females, 2);
  ASSERT_EQUAL(stats.employed_males, 4);
  ASSERT_EQUAL(stats.unemployed_males, 5);
}

void TestPrintStats() {
  ostringstream os;
  AgeStats stats {7, 6, 5, 4, 3, 2, 1};
  PrintStats(stats, os);

}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestComputeMedianAgeVoidRange);
  RUN_TEST(tr, TestComputeMedianAgeOddSizeRange);
  RUN_TEST(tr, TestReadPersons);
  RUN_TEST(tr, TestComputeStats);
  RUN_TEST(tr, TestPrintStats);
}
