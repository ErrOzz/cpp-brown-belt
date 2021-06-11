//#include "test_runner.h"

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

  static const int DAY_DURATION = 86400;

  void RemoveOldBookings(const long long int& current_time) {
    while (day_que.size() &&
           day_que.front().time + DAY_DURATION  <= current_time) {
      auto client_it = client_appears.find(day_que.front().client_id);
      if (--client_it->second == 0) {
        client_appears.erase(client_it);
        --client_count;
      }
      room_count -= day_que.front().room_count;
      day_que.pop_front();
    }
  }
public:
  void Book(Book_Item book) {
    day_que.push_back(move(book));
    ++client_appears[book.client_id];
    if (client_appears[book.client_id] == 1) ++client_count;
    room_count += book.room_count;
  }
  int Clients(const long long int& current_time) {
    RemoveOldBookings(current_time);
    return client_count;
  }
  int Rooms(const long long int& current_time) {
    RemoveOldBookings(current_time);
    return room_count;
  }
};

class BookManager {
  unordered_map<string, HotelManager> bookings;
  long long int current_time;
public:
  void Book(long long int time, string hotel_name,
            int client_id, int rooms) {
    bookings[move(hotel_name)].Book({time, client_id, rooms});
    current_time = time;
  }
  int Clients(const string& hotel_name) {
    return bookings[hotel_name].Clients(current_time);
  }
  int Rooms(const string& hotel_name) {
    return bookings[hotel_name].Rooms(current_time);
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
