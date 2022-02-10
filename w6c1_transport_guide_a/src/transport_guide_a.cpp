#include <iostream>
#include <cmath>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

struct Request;
using RequestHolder = unique_ptr<Request>;

struct Request {
  enum class Type {
    ADD_STOP,
    ADD_BUS,
    BUS_INFO
  };

  const Type type;

  Request(Type type) : type(type) {}
  static RequestHolder Create(Type type);
  virtual void ParseFrom(string_view input) = 0;
  virtual ~Request() = default;
};

unordered_map<string_view, Request::Type> STR_TO_REQUEST_TYPE = {
    {"Stop", Request::Type::ADD_STOP},

};

#ifdef TESTS
#include "tests/test_all.h"
#endif
int main() {
#ifdef TESTS
  TEST_ALL();
#endif

  return 0;
}
