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

// reference version

//bool IsSubdomain(const Rstring& subdomain, const Rstring& domain) {
//  int i = subdomain.size() - 1;
//  int j = domain.size() - 1;
//  while(i >=0 && j >= 0) {
//    if(subdomain[i--] != domain[j--]) {
//      return false;
//    }
//  }
//  return (i < 0 && j < 0) ||
//         (i < 0 && domain[j] == '.') ||
//         (j < 0 && subdomain[i] == '.');
//}

bool IsSubdomain(const Rstring& subdomain, const Rstring& domain) {
  if(const size_t subdomain_size = subdomain.size(),
                  domain_size = domain.size(),
                  pos = subdomain_size - domain_size;
     subdomain_size >= domain_size &&
     subdomain.find(domain, pos) != Rstring::npos &&
     (pos ? (subdomain.at(pos - 1) == '.') : true)) {
    return true;
  }
  return false;
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
    if(IsSubdomain(*it, *stored_it)) {
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
  if(it != denied.begin() && IsSubdomain(domain, *prev(it))) {
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
