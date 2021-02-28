#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <optional>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
  IteratorRange(Iterator begin, Iterator end)
    : first(begin)
    , last(end)
  {
  }

  Iterator begin() const {
    return first;
  }

  Iterator end() const {
    return last;
  }

private:
  Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
  return IteratorRange<Collection>{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person {
  string name;
  int age, income;
  bool is_male;
};

vector<Person> ReadPeople(istream& input) {
  int count;
  input >> count;

  vector<Person> result(count);
  for (Person& p : result) {
    char gender;
    input >> p.name >> p.age >> p.income >> gender;
    p.is_male = gender == 'M';
  }

  return result;
}

template <typename Iter>
const std::optional<string> MostPopularName(const IteratorRange<Iter>& range) {
  if (range.begin() == range.end()) {
    return std::nullopt;
  } else {
    sort(range.begin(), range.end(), [](const Person& lhs, const Person& rhs) {
      return lhs.name < rhs.name;
    });
    const string* most_popular_name = &range.begin()->name;
    int count = 1;
    for (auto it = range.begin(); it != range.end(); ) {
      auto same_name_end = find_if_not(it, range.end(), [it](const Person& p) {
        return p.name == it->name;
      });
      auto cur_name_count = std::distance(it, same_name_end);
      if (cur_name_count > count) {
        count = cur_name_count;
        most_popular_name = &it->name;
      }
      it = same_name_end;
    }
    return *most_popular_name;
  }
}

int main() {
  vector<Person> people = ReadPeople(cin);
  IteratorRange<vector<Person>::iterator> range{people.begin(),
    partition(people.begin(), people.end(), [](const Person& p) {
      return p.is_male;
    })
  };
  const std::optional<string> most_pop_name_M = MostPopularName(range);
  range = IteratorRange<vector<Person>::iterator>{range.end(), people.end()};
  const std::optional<string> most_pop_name_F = MostPopularName(range);

  const vector<Person> age_sorted = [&people]{
    sort(begin(people), end(people), [](const Person& lhs, const Person& rhs) {
      return lhs.age < rhs.age;
    });
    return move(people);
  }();

  const vector<int> wealthy_sorted = [&age_sorted] {
    vector<int> result;
    result.reserve(age_sorted.size());
    for (const auto& p : age_sorted) {
      result.push_back(p.income);
    }
    sort(result.begin(), result.end(), [](int lhs, int rhs) {
      return lhs > rhs;
    });
    return result;
  }();

  for (string command; cin >> command; ) {
    if (command == "AGE") {
      int adult_age;
      cin >> adult_age;

      auto adult_begin = lower_bound(
        begin(age_sorted), end(age_sorted), adult_age, [](const Person& lhs, int age) {
          return lhs.age < age;
        }
      );

      cout << "There are " << std::distance(adult_begin, end(age_sorted))
           << " adult people for maturity age " << adult_age << '\n';
    } else if (command == "WEALTHY") {
      size_t count;
      cin >> count;
      int total_income = accumulate(wealthy_sorted.begin(),
        wealthy_sorted.begin() + min(count, wealthy_sorted.size()), 0);
      cout << "Top-" << count << " people have total income " << total_income << '\n';
    } else if (command == "POPULAR_NAME") {
      char gender;
      cin >> gender;

      const std::optional<string>& most_popular_name = gender == 'M' ? most_pop_name_M : most_pop_name_F;
      if (!most_popular_name.has_value()) {
        cout << "No people of gender " << gender << '\n';
      } else {
        cout << "Most popular name among people of gender " << gender << " is "
             << *most_popular_name << '\n';
      }
    }
  }
}
