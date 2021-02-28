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

struct StatsData {
  std::optional<string> most_popular_M_name;
  std::optional<string> most_popular_F_name;
  vector<int> cumulative_wealth;
  vector<Person> age_sorted;
};

StatsData BuildSatatsData(vector<Person> people) {
  StatsData result;
  {
    IteratorRange<vector<Person>::iterator> range{people.begin(),
      partition(people.begin(), people.end(), [](const Person& p) {
        return p.is_male;
      })
    };
    result.most_popular_M_name = MostPopularName(range);
    range = IteratorRange<vector<Person>::iterator>{range.end(), people.end()};
    result.most_popular_F_name = MostPopularName(range);
  }

  {
    sort(people.begin(), people.end(), [](const Person& lhs, const Person& rhs) {
      return lhs.income > rhs.income;
    });
    vector<int>& wealth = result.cumulative_wealth;
    wealth.resize(people.size());
    if (people.size()) {
      wealth[0] = people[0].income;
      for (size_t i = 1; i < people.size(); ++i) {
        wealth[i] = wealth[i - 1] + people[i].income;
      }
    }
  }

  sort(begin(people), end(people), [](const Person& lhs, const Person& rhs) {
    return lhs.age < rhs.age;
  });
  result.age_sorted = std::move(people);
  return result;
}

int main() {
  const StatsData stats = BuildSatatsData(ReadPeople(cin));

  for (string command; cin >> command; ) {
    if (command == "AGE") {
      int adult_age;
      cin >> adult_age;

      auto adult_begin = lower_bound(
        begin(stats.age_sorted), end(stats.age_sorted),
              adult_age, [](const Person& lhs, int age) {
          return lhs.age < age;
        }
      );

      cout << "There are " << std::distance(adult_begin, end(stats.age_sorted))
           << " adult people for maturity age " << adult_age << '\n';
    } else if (command == "WEALTHY") {
      size_t count;
      cin >> count;

      cout << "Top-" << count << " people have total income "
           << stats.cumulative_wealth[count - 1] << '\n';
    } else if (command == "POPULAR_NAME") {
      char gender;
      cin >> gender;

      const std::optional<string>& most_popular_name
        = gender == 'M' ? stats.most_popular_M_name
                        : stats.most_popular_F_name;
      if (most_popular_name) {
        cout << "Most popular name among people of gender " << gender << " is "
             << *most_popular_name << '\n';
      } else {
        cout << "No people of gender " << gender << '\n';
      }
    }
  }
}
