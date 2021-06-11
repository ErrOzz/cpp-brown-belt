#include "test_runner.h"

#include <iostream>
#include <list>
#include <map>
#include <unordered_map>
#include <string>

using namespace std;

struct Book_Item{
  long long int time;
  int client_id;
  int room_count;
};

class HotelManager {
  list<Book_Item> day_que;
  map<int, int> client_appears;
  int client_count = 0;
  int room_count = 0;

  static const int day_duration = 86400;

  void RemoveOldBookings() {
    long long int current_time = day_que.back().time;
    for (auto& front_book = day_que.front();
           front_book.time + day_duration  <= current_time;) {
      auto client_it = client_appears.find(front_book.client_id);
      if (--client_it->second == 0) {
        client_appears.erase(client_it);
        --client_count;
      }
      room_count -= front_book.room_count;
      day_que.pop_front();
    }
  }
public:
  void Book(long long int time, int client_id, int rooms) {
    day_que.push_back({time, client_id, rooms});
    ++client_appears[client_id];
    if (client_appears[client_id] == 1) ++client_count;
    room_count += rooms;
    RemoveOldBookings();
  }
  int Clients() const {
    return client_count;
  }
  int Rooms() const {
    return room_count;
  }
};

class BookManager {
  unordered_map<string, HotelManager> bookings;
public:
  void Book(long long int time, string hotel_name,
            int client_id, int rooms) {
    bookings[move(hotel_name)].Book(time, client_id, rooms);
  }
  int Clients(const string& hotel_name) const {
    auto it = bookings.find(hotel_name);
    if (it == bookings.end()) return 0;
    return it->second.Clients();
  }
  int Rooms(const string& hotel_name) const {
    auto it = bookings.find(hotel_name);
    if (it == bookings.end()) return 0;
    return it->second.Rooms();
  }
};

int main() {
  int q;
  cin >> q;
  BookManager bm;
  do {
    string command, hotel_name;
    cin >> command;
    if (command == "BOOK") {
      long long int time;
      int client_id, rooms;
      cin >> time >> hotel_name >> client_id >> rooms;
      bm.Book(time, move(hotel_name), client_id, rooms);
    } else if (command == "CLIENTS") {
      cin >> hotel_name;
      cout << bm.Clients(hotel_name) << endl;
    } else if (command == "ROOMS") {
      cin >> hotel_name;
      cout << bm.Rooms(hotel_name) << endl;
    }
  } while (--q > 0);
  return 0;
}
