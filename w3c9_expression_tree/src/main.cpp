#include "Common.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

namespace Expr {

class Value : public Expression {
private:
  const int value;
public:
  explicit Value(int value) : value(value) {}
  virtual int Evaluate() const override {
    return value;
  }

  virtual string ToString() const override {
    stringstream output;
    output << '(' << value << ')';
    return output.str();
  }
};

class Sum : public Expression {
private:
  ExpressionPtr lhs, rhs;
public:
  Sum(ExpressionPtr lhs, ExpressionPtr rhs) : lhs(move(lhs)), rhs(move(rhs)) {}
  virtual int Evaluate() const override {
    return lhs->Evaluate() + rhs->Evaluate();
  }
  virtual string ToString() const override {
    stringstream output;
    output << lhs->ToString() << '+' << rhs->ToString();
    return output.str();
  }
};

class Product : public Expression {
private:
  ExpressionPtr lhs, rhs;
public:
  Product(ExpressionPtr lhs, ExpressionPtr rhs) : lhs(move(lhs)), rhs(move(rhs)) {}
  virtual int Evaluate() const override {
    return lhs->Evaluate() * rhs->Evaluate();
  }
  virtual string ToString() const override {
    stringstream output;
    output << lhs->ToString() << '*' << rhs->ToString();
    return output.str();
  }
};

}

ExpressionPtr Value(int value) {
  return make_unique<Expr::Value>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<Expr::Sum>(move(left), move(right));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<Expr::Product>(move(left), move(right));
}

string Print(const Expression* e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

void TestSimple() {
  ExpressionPtr v1 = Value(18);
  ASSERT_EQUAL(Print(v1.get()), "(18) = 18");
  ExpressionPtr v2 = Value(22);
  ASSERT_EQUAL(Print(v2.get()), "(22) = 22");
  ExpressionPtr s1 = Sum(move(v1), move(v2));
  ASSERT_EQUAL(Print(s1.get()), "(18)+(22) = 40");

}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSimple);
  RUN_TEST(tr, Test);
  return 0;
}
