#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

class ReadingManager {
  static const int MAX_USER_COUNT = 100'000;
  static const int MAX_PAGE_COUNT = 1'000;

  vector<int> user_to_page;
  vector<int> page_to_user;
public:
  ReadingManager()
      : user_to_page(MAX_USER_COUNT, 0),
        page_to_user(MAX_PAGE_COUNT, 0) {}

  void Read(int user_id, int page_count) {
    
  }

};

int main() {
  // Для ускорения чтения данных отключается синхронизация
  // cin и cout с stdio,
  // а также выполняется отвязка cin от cout
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  ReadingManager manager;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int user_id;
    cin >> user_id;

    if (query_type == "READ") {
      int page_count;
      cin >> page_count;
      manager.Read(user_id, page_count);
    } else if (query_type == "CHEER") {
      cout << setprecision(6) << manager.Cheer(user_id) << "\n";
    }
  }

  return 0;
}
