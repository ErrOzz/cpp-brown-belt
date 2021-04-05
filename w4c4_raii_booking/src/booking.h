namespace RAII {

template <typename Provider>
class Booking {
  using Id = typename Provider::BookingId;

  Provider* provider;
  Id id;
public:
  Booking(Provider* provider, Id id) : provider(provider), id(id) {}
  ~Booking() {
    provider->CancelOrComplete(*this);
  }
};

}
