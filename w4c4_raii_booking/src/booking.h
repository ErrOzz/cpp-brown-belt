#include <memory>
#include <functional>

namespace RAII {

// Using bool constructed attribute in destructor

//template <typename Provider>
//class Booking {
//  using Id = typename Provider::BookingId;
//
//  Provider* provider;
//  Id id;
//  bool constructed;
//public:
//  Booking(Provider* provider, Id id) : provider(provider), id(id), constructed(true) {}
//  Booking(const Booking&) = delete;
//  Booking(Booking&& other) : provider(other.provider), id(other.id), constructed(other.constructed) {
//    other.provider = nullptr;
//    other.constructed = false;
//  }
//  Booking& operator=(const Booking&) = delete;
//  Booking& operator=(Booking&& other) {
//    provider = other.provider;
//    other.provider = nullptr;
//    id = other.id;
//    constructed = other.constructed;
//    other.constructed = false;
//    return *this;
//  }
//  ~Booking() {
//    if (constructed) provider->CancelOrComplete(*this);
//  }
//};

// Using unique_ptr custom deleter

#define PRV_DELETER [this](Provider* prv) { \
    prv->CancelOrComplete(*this);           \
    std::default_delete<Provider>();        \
  })

template <typename Provider>
class Booking {
  using Id = typename Provider::BookingId;
  using deleted_unique_ptr = std::unique_ptr<Provider, std::function<void(Provider*)>>;


  deleted_unique_ptr provider_;
  Id id_;
public:
  Booking(Provider* provider, Id id) : provider_(deleted_unique_ptr(provider, PRV_DELETER), id_(id) {
  }
  Booking(const Booking&) = delete;
  Booking(Booking&& other) = default;
  Booking& operator=(const Booking&) = delete;
  Booking& operator=(Booking&& other) = default;
};

}
