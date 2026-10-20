#include "library/utility/fastio.hpp"

#include <string>

#include "library/string/rolling_hash.hpp"

int main() {
  std::string s;
  soryuusi::cin >> s;
  soryuusi::RollingHash<1> rolhash(s);
  for (int i = 0; i < static_cast<int>(s.size()); ++i) {
    soryuusi::cout << rolhash.lcp(0,i) << " \n"[i + 1 == static_cast<int>(s.size())];
  }
}
