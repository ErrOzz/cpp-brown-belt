#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};


class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // только первому worker-у в пайплайне нужно это имплементировать
    throw logic_error("Unimplemented");
  }

protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
  void PassOn(unique_ptr<Email> email) const {
    if (next_w) next_w.Process(move(email));
  }

public:
  void SetNext(unique_ptr<Worker> next) {
    if (next) next_w = move(next);
  }
private:
  unique_ptr<worker> next_w = nullptr;
};


class Reader : public Worker {
public:
  explicit Reader(istream& input) {
    input_r = input;
  }
  void Run() {
    while (input) {
      ...
      auto email = make_unique<Email>();
      PassOn(email);
    }
  }
private:
  istream& input_r;
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;

public:
  // реализуйте класс
};


class Copier : public Worker {
public:
  // реализуйте класс
};


class Sender : public Worker {
public:
  // реализуйте класс
};


// реализуйте класс
class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in);

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter);

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(string recipient);

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out);

  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build();
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
  );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
  });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
  );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
