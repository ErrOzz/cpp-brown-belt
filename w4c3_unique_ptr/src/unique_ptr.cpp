#include "test_runner.h"

#include <cstddef>  // нужно для nullptr_t

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr {
private:
  T* ptr_ = nullptr;
public:
  UniquePtr() {}
  UniquePtr(T* ptr) : ptr_(ptr) {}
  UniquePtr(const UniquePtr&) = delete;
  UniquePtr(UniquePtr&& other) : ptr_(other.Release()) {}
  UniquePtr& operator = (const UniquePtr&) = delete;
  UniquePtr& operator = (nullptr_t) {
    delete Release();
    return *this;
  }
  UniquePtr& operator = (UniquePtr&& other) {
    if (this->ptr_) { delete ptr_; }
    ptr_ = other.Release();
    return *this;
  }
  ~UniquePtr() { delete ptr_; }

  T& operator * () const { return *ptr_; }

  T* operator -> () const { return ptr_; }

  T* Release() {
    T* result = nullptr;
    swap(ptr_, result);
    return result;
  }

  void Reset(T * ptr) {
    delete ptr_;
    ptr_ = ptr;
  }

  void Swap(UniquePtr& other) {
    swap(ptr_, other.ptr_);
  }

  T * Get() const { return ptr_; }
};


struct Item {
  static int counter;
  int value;
  Item(int v = 0): value(v) {
    ++counter;
  }
  Item(const Item& other): value(other.value) {
    ++counter;
  }
  ~Item() {
    --counter;
  }
};

int Item::counter = 0;


void TestLifetime() {
  Item::counter = 0;
  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ptr.Reset(new Item);
    ASSERT_EQUAL(Item::counter, 1);
  }
  ASSERT_EQUAL(Item::counter, 0);

  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    auto rawPtr = ptr.Release();
    ASSERT_EQUAL(Item::counter, 1);

    delete rawPtr;
    ASSERT_EQUAL(Item::counter, 0);
  }
  ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
  UniquePtr<Item> ptr(new Item(42));
  ASSERT_EQUAL(ptr.Get()->value, 42);
  ASSERT_EQUAL((*ptr).value, 42);
  ASSERT_EQUAL(ptr->value, 42);
}

void TestMove() {
  UniquePtr<Item> one(new Item(41));
  ASSERT_EQUAL(Item::counter, 1);

  UniquePtr<Item> two(new Item(14));
  ASSERT_EQUAL(Item::counter, 2);

  one = move(two);
  ASSERT_EQUAL(Item::counter, 1);
  ASSERT_EQUAL(one->value, 14);
  ASSERT(two.Get() == nullptr);

  UniquePtr<Item> three(move(one));
  ASSERT_EQUAL(Item::counter, 1);
  ASSERT_EQUAL(three->value, 14);
  ASSERT(one.Get() == nullptr);
}

void TestNull() {
  UniquePtr<Item> ptr(new Item(33));
  ASSERT_EQUAL(Item::counter, 1);
  ptr = nullptr;
  ASSERT(ptr.Get() == nullptr);
  ASSERT_EQUAL(Item::counter, 0);
}

void TestReset() {
  UniquePtr<Item> ptr{};
  ASSERT_EQUAL(Item::counter, 0);
  ptr.Reset(new Item(17));
  ASSERT_EQUAL(Item::counter, 1);
  ptr.Reset(new Item(18));
  ASSERT_EQUAL(Item::counter, 1);
  ptr.Reset(nullptr);
  ASSERT_EQUAL(Item::counter, 0);
}

void TestRelease() {
  UniquePtr<Item> ptr{};
  ASSERT(ptr.Release() == nullptr);
  ptr.Reset(new Item);
  ASSERT_EQUAL(Item::counter, 1);
  auto rawPtr = ptr.Release();
  ASSERT_EQUAL(Item::counter, 1);
  delete rawPtr;
  ASSERT_EQUAL(Item::counter, 0);
  ASSERT(ptr.Get() == nullptr);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
  RUN_TEST(tr, TestMove);
  RUN_TEST(tr, TestNull);
  RUN_TEST(tr, TestReset);
  RUN_TEST(tr, TestRelease);
}
