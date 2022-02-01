//#define TEST_RUNNER
#ifdef TEST_RUNNER
#include "test_runner.h"
#endif

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <set>

using namespace std;

class RouteManager {
public:
  void AddRoute(int start, int finish) {
    reachable_lists_[start].insert(finish);
    reachable_lists_[finish].insert(start);
  }
  int FindNearestFinish(int start, int finish) const {
    int result = abs(start - finish);
    if (reachable_lists_.count(start) < 1) {
        return result;
    }
    const set<int>& reachable_stations = reachable_lists_.at(start);
    const auto it = reachable_stations.lower_bound(finish);
    if (it != end(reachable_stations)) {
      result = min(result, abs(finish - *it));
    }
    if (it != begin(reachable_stations)) {
      result = min(result, abs(*prev(it) - finish));
    }
    return result;
  }
private:
  map<int, set<int>> reachable_lists_;
};
#ifdef TEST_RUNNER
void TestOne() {
  RouteManager rm;
  rm.AddRoute(0, 10);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 100), 90);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 22), 12);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 13), 3);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 10), 0);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 9), 1);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 5), 5);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 2), 2);
  ASSERT_EQUAL(rm.FindNearestFinish(0, -2), 2);
  ASSERT_EQUAL(rm.FindNearestFinish(0, -10), 10);
  ASSERT_EQUAL(rm.FindNearestFinish(0, -50), 50);
  ASSERT_EQUAL(rm.FindNearestFinish(0, -100), 100);

  ASSERT_EQUAL(rm.FindNearestFinish(10, -100), 100);
  ASSERT_EQUAL(rm.FindNearestFinish(10, -10), 10);
  ASSERT_EQUAL(rm.FindNearestFinish(10, -5), 5);
  ASSERT_EQUAL(rm.FindNearestFinish(10, -1), 1);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 0), 0);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 1), 1);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 5), 5);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 6), 4);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 9), 1);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 11), 1);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 20), 10);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 100), 90);
}

void TestSome() {
  RouteManager rm;
  rm.AddRoute(0, 10);
  rm.AddRoute(0, 12);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 10), 0);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 0), 0);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 12), 0);
  ASSERT_EQUAL(rm.FindNearestFinish(12, 0), 0);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 8), 2);
  ASSERT_EQUAL(rm.FindNearestFinish(8, 0), 8);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 14), 2);
  ASSERT_EQUAL(rm.FindNearestFinish(14, 0), 14);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 12), 2);
  ASSERT_EQUAL(rm.FindNearestFinish(12, 10), 2);

  ASSERT_EQUAL(rm.FindNearestFinish(0, 100), 88);
  ASSERT_EQUAL(rm.FindNearestFinish(10, 100), 90);
  ASSERT_EQUAL(rm.FindNearestFinish(12, 100), 88);
  ASSERT_EQUAL(rm.FindNearestFinish(0, -100), 100);
  rm.AddRoute(0, -5);
  ASSERT_EQUAL(rm.FindNearestFinish(0, -100), 95);
  ASSERT_EQUAL(rm.FindNearestFinish(-5, 12), 12);
  ASSERT_EQUAL(rm.FindNearestFinish(12, -5), 5);
  ASSERT_EQUAL(rm.FindNearestFinish(-5, -15), 10);

  ASSERT_EQUAL(rm.FindNearestFinish(12, 12), 0);
}

void TestMultiple() {
  RouteManager rm;
  rm.AddRoute(0, -5);
  rm.AddRoute(0, -50);
  rm.AddRoute(0, 150);
  rm.AddRoute(0, 200);
  rm.AddRoute(0, 230);
  ASSERT_EQUAL(rm.FindNearestFinish(0, 70), 70);
}
#endif
int main() {
#ifdef TEST_RUNNER
  TestRunner tr;
  RUN_TEST(tr, TestOne);
  RUN_TEST(tr, TestSome);
  RUN_TEST(tr, TestMultiple);
#else
  RouteManager routes;
  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int start, finish;
    cin >> start >> finish;
    if (query_type == "ADD") {
      routes.AddRoute(start, finish);
    } else if (query_type == "GO") {
      cout << routes.FindNearestFinish(start, finish) << "\n";
    }
  }
#endif
  return 0;
}
