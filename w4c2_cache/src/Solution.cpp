#include "Common.h"
#include <unordered_map>
#include <mutex>
#include <algorithm>

#include<iostream>

using namespace std;

struct BookAttr {
  ICache::BookPtr ptr;
  size_t rating;
};

class LruCache : public ICache {
  unordered_map<string, BookAttr> data_;
  const shared_ptr<IBooksUnpacker> unpacker_;
  const Settings settings_;
  size_t cur_memory = 0;
  size_t max_rating = 0;
  unique_ptr<mutex> mutex_ptr = make_unique<mutex>();

  void FreeMemory(size_t size) {
    lock_guard<mutex> lock(*mutex_ptr);
    while (settings_.max_memory - cur_memory < size && cur_memory > 0) {
      auto min_it = min_element(data_.begin(), data_.end(),
                                [](const auto& lhs, const auto& rhs){
        return lhs.second.rating < rhs.second.rating;
      });
      const auto size_to_free = min_it->second.ptr->GetContent().size();
      data_.erase(min_it);
      cur_memory -= size_to_free;
    }
  }
public:
  LruCache(shared_ptr<IBooksUnpacker> books_unpacker,
           const Settings& settings) : unpacker_(move(books_unpacker))
                                     , settings_(settings) {
    // реализуйте метод
  }

  BookPtr GetBook(const string& book_name) override {
    if (const auto it = data_.find(book_name); it != data_.end()) {
      {
        lock_guard<mutex> lock(*mutex_ptr);
        it->second.rating = ++max_rating;
      }
      return it->second.ptr;
    }
    auto new_book = unpacker_->UnpackBook(book_name);
    auto new_book_size = new_book->GetContent().size();
    FreeMemory(new_book_size);
    if (new_book_size > settings_.max_memory) return move(new_book);
    {
      lock_guard<mutex> lock(*mutex_ptr);
      cur_memory += new_book_size;
      data_[book_name] = {move(new_book), ++max_rating};
    }
    return data_[book_name].ptr;
  }
};


unique_ptr<ICache> MakeCache(
    shared_ptr<IBooksUnpacker> books_unpacker,
    const ICache::Settings& settings
) {
  return make_unique<LruCache>(LruCache(move(books_unpacker), settings));
}
