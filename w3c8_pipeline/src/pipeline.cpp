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
  explicit Reader(istream& input) : input_r(input) {}
  void Process(unique_ptr<Email> email) override {}
  void Run() override {
    while (input_r) {
      auto email = make_unique<Email>();
      getline(input_r, email->from);
      getline(input_r, email->to);
      getline(input_r, email->body);
      PassOn(move(email));
    }
  }
private:
  istream& input_r;
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;
  explicit Filter(Function function) : function_f(move(function)) {}
  void Process(unique_ptr<Email> email) override {
    if (function_f && function_f(email)) PassOn(move(email));
  }
private:
  Function function_f;
};


class Copier : public Worker {
public:
  explicit Copier(string recipient) : recipient_c(move(recipient)) {}
  void Process(unique_ptr<Email> email) override {
    if (email.to != recipient_c) {
      auto copy = make_unique<Email>({email.from, recipient_c, email.body});
      PassOn(move(copy));
    }
    PassOn(move(email));
  }
private:
  string recipient_c;
};


class Sender : public Worker {
public:
  explicit Sender(ostream& output) : output_s(output) {}
  void Process(unique_ptr<Email> email) override {
    output << email.from << '\n'
           << email.to << '\n'
           << email.body << '\n';
  }
private:
  ostream& output_s;
};


// реализуйте класс
class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in) {
    pipeline = make_unique<Reader>(in);
  }

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter) {
    pipeline
    return *this;
  }

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(string recipient);

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out);

  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build();
private:
  unique_ptr<Worker> pipline;
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
