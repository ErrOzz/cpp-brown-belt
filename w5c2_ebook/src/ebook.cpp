#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

class ReadingManager {
  static const int MAX_USER_COUNT = 100'000;
  static const int MAX_PAGE_COUNT = 1'000;

  vector<int> user_to_page;
  vector<int> page_to_users;
public:
  ReadingManager()
      : user_to_page(MAX_USER_COUNT + 1, 0),
        page_to_users(MAX_PAGE_COUNT + 1, 0) {}

  void Read(int user_id, int page_count) {
    const int old_page = user_to_page[user_id];
    --page_to_users[old_page];
    ++page_to_users[page_count];
    user_to_page[user_id] = page_count;
  }

  double Cheer(int user_id) const {
    const int page = user_to_page[user_id];
    const int user_count = UserCount();
    if (!page) { return 0; }
    if (user_count == 1) { return 1; }
    int low_rated_user_count = 0;
    for (int i = 1; i < page; ++i) {
      low_rated_user_count += page_to_users[i];
    }
    return low_rated_user_count * 1.0 / (user_count - 1);
  }

private:
  int UserCount() const {
    return -page_to_users[0];
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
