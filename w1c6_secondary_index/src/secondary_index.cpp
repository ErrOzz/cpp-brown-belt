#include "test_runner.h"
#include "profile.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <random>
#include <algorithm>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

class Database {
public:
  bool Put(const Record& record) {
    Id id = record.id;
    if (data.count(id)) return false;
    Record* record_ptr = &(data[id] = record);
    timestamp_sorted.insert({record.timestamp, record_ptr});
    karma_sorted.insert({record.karma, record_ptr});
    name_sorted.insert({record.user, record_ptr});
    return true;
  }
  const Record* GetById(const string& id) const {
    if (!data.count(id)) return nullptr;
    return &(data.at(id));
  }
  bool Erase(const string& id) {
    if (!data.count(id)) return false;
    Record& record = data.at(id);
    RemoveSortedId(timestamp_sorted, record.timestamp, id);
    RemoveSortedId(karma_sorted, record.karma, id);
    RemoveSortedId(name_sorted, record.user, id);
    data.erase(id);
    return true;
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) {
    RangeBy(timestamp_sorted, low, high, callback);
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
    RangeBy(karma_sorted, low, high, callback);
  }

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const {
    RangeBy(name_sorted, user, user, callback);
  }

private:
  using Id = string;
  unordered_map<Id, Record> data;
  multimap<int, Record*> timestamp_sorted;
  multimap<int, Record*> karma_sorted;
  multimap<string, Record*> name_sorted;

  template <typename Container, typename Key>
  void RemoveSortedId(Container& container, Key& key, const Id& id) {
    auto range = container.equal_range(key);
    for (auto i = range.first; i!= range.second; ++i) {
      if (i->second->id == id) {
        container.erase(i);
        break;
      }
    }
  }

  template <typename Container, typename Key, typename Callback>
  void RangeBy(const Container& container,
               const Key& low, const Key& high,
               Callback callback) const {
    auto begin_it = container.lower_bound(low);
    const auto end_it = container.upper_bound(high);
    while (begin_it != end_it) {
      if (!callback(*(begin_it->second))) break;
      ++begin_it;
    }
  }
};

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
  db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
  db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

  int count = 0;
  db.AllByUser("master", [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
  db.Erase("id");
  db.Put({"id", final_body, "not-master", 1536107260, -10});

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

void TestRandom() {
  default_random_engine rd(37);
  uniform_int_distribution<char> char_gen('a', 'z');

  auto random_word = [&](size_t len) {
    string result(len, ' ');
    generate(result.begin(), result.end(), [&] { return char_gen(rd); });
    return result;
  };

#define TIMESTAMPS 50000
  uniform_int_distribution<size_t> id_len_gen(4, 6);
  uniform_int_distribution<size_t> title_len_gen(5, 21);
  uniform_int_distribution<size_t> user_len_gen(4, 8);
  uniform_int_distribution<int> timestamp_gen(0, TIMESTAMPS);
  uniform_int_distribution<int> karma_gen(-100, 100);

#define VALUE 100000
  vector<Record> records;
  records.reserve(VALUE);
  while (records.size() < VALUE) {
    records.push_back({ random_word(id_len_gen(rd)),
                        random_word(user_len_gen(rd)),
                        random_word(title_len_gen(rd)),
                        timestamp_gen(rd),
                        karma_gen(rd) });
  }

  Database db;
  { LOG_DURATION("Put records to Database")
  for (const auto& record : records) {
    db.Put(record);
  }
  }
  vector<string> ids;
  ids.reserve(VALUE);
  { LOG_DURATION("Full RangeByKarma iterating")
  db.RangeByKarma(-100, 100, [&ids](const Record& record) {
    ids.push_back(record.id);
    return true;
  });
  }
  cerr << "Result Database size: " << ids.size() << endl;
  vector<int> timestamps(TIMESTAMPS, 0);
  { LOG_DURATION("Full RangeByTimestamp iterating")
  db.RangeByTimestamp(0, TIMESTAMPS, [&timestamps](const Record& record) {
    ++timestamps[record.timestamp];
    return true;
  });
  }
  size_t time_count = 0;
  size_t time_uniq = 0;
  size_t time_overlaps = 0;
  for (const auto & stamps : timestamps) {
    if (stamps) ++time_count;
    if (stamps > 1) time_overlaps += stamps - 1;
    if (stamps == 1) ++time_uniq;
  }
  cerr << "Result Timestamps count: " << time_count << endl;
  cerr << "Result Timestamps overlaps: " << time_overlaps << endl;
  cerr << "Result Timestamps uniq: " << time_uniq << endl;
  { LOG_DURATION("Erasing all records from Database")
  for (const auto& id : ids) {
    db.Erase(id);
  }
  }
  bool fail = false;
  for (const auto& id : ids) {
    if (db.GetById(id) != nullptr) {
      fail = true;
    }
    ASSERT(!fail);
  }
  /*  Put records to Database: 700 - 800 ms
   *  Full RangeByKarma iterating: 70 - 100 ms
   *  Full RangeByTimestamp iterating: 70 - 100 ms
   *  Erasing all records from Database: 700 - 800 ms
   */
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  RUN_TEST(tr, TestRandom);
  return 0;
}
