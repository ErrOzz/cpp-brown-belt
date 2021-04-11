#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <unordered_set>
using namespace std;

// Добавили свои классы для хеширования и сравнения ключей
struct Hash {
  using is_transparent = void;
  template <class P>
  size_t operator()(const P& p) const {
    return hash<P>()(p);
  }
};
struct KeyEqual {
  using is_transparent = void;
  template <class P, class Q>
  bool operator()(const P& lhs, const Q& rhs) const {
    return to_address(lhs) == to_address(rhs);
  }
};

template <class T>
class ObjectPool {
public:
  T* Allocate();
  T* TryAllocate();

  void Deallocate(T* object);

private:
  // Убрали свой компаратор

  queue<unique_ptr<T>> free;
  // Изменили на unordered_set с соответствующими параметрами
  unordered_set<unique_ptr<T>, Hash, KeyEqual> allocated;
};

template <typename T>
T* ObjectPool<T>::Allocate() {
  if (free.empty()) {
    free.push(make_unique<T>());
  }
  auto ptr = move(free.front());
  free.pop();
  T* ret = ptr.get();
  allocated.insert(move(ptr));
  return ret;
}

template <typename T>
T* ObjectPool<T>::TryAllocate() {
  if (free.empty()) {
    return nullptr;
  }
  return Allocate();
}

// Убрали функции сравнения, они больше не нужны

template <typename T>
void ObjectPool<T>::Deallocate(T* object) {
  auto it = allocated.find(object);
  if (it == allocated.end()) {
    throw invalid_argument("");
  }
  free.push(move(allocated.extract(it).value()));
}

int main() {
  return 0;
}
