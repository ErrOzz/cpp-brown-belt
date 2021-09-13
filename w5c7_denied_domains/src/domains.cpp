#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <set>

using namespace std;

class Rstring : public string {
};

bool operator<(const Rstring& lhs, const Rstring& rhs) {
  return lexicographical_compare(lhs.rbegin(), lhs.rend(), rhs.rbegin(), rhs.rend());
}

set<Rstring> ReadDomains(istream& input, int number) {
  set<Rstring> result;
  for (int i = 0; i < number; ++i) {
    Rstring line;
    getline(input, line);
    result.insert(move(line));
  }
  return result;
}

set<Rstring> FilterDomains(set<Rstring>&& domains) {
  vector<set<Rstring>::iterator> domains_to_delete;
  for (auto stored_it = domains.begin(), it = next(stored_it); it != domains.end(); ) {
    if (size_t domain_size = it->size(),
               stored_size = stored_it->size(),
               pos = domain_size - stored_size;
        domain_size > stored_size &&
        it->find(*stored_it, pos) != Rstring::npos &&
        it->at(pos - 1) == '.') {
      domains_to_delete.push_back(it);
    } else {
      stored_it = it;
    }
    it = next(it);
  }
  for (auto it : domains_to_delete) {
    domains.erase(it);
  }
  return move(domains);
}

bool VerifyDomain(istream& input, const set<Rstring>& denied) {
  Rstring domain;
  getline(input, domain);
  const auto it = denied.upper_bound(domain);
  if(it == denied.begin()) {
    return true;
  }
  if(const size_t domain_size = domain.size(),
                  denied_size = prev(it)->size(),
                  pos = domain.rfind(*prev(it));
     domain_size >= denied_size &&
     pos != Rstring::npos &&
     pos == domain_size - denied_size &&
     (pos ? (domain[pos - 1] == '.') : true)) {
    return false;
  }
  return true;
}

int main() {
  int q;
  cin >> q;
  //  deletes "/n" simbol in cin
  cin.get();
  auto denied_domains = FilterDomains(ReadDomains(cin, q));
  cin >> q;
  cin.get();
  for (int i = 0; i < q; ++i) {
    cout << (VerifyDomain(cin, denied_domains) ? "Good" : "Bad") << endl;
  }
  return 0;
}
