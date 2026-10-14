#include "library/utility/fastio.hpp"
#include <iostream>
#include <string>

#include "library/utility/64_ary_tree.hpp"

int main() {

  int n, q;
  std::string initial;
  soryuusi::cin >> n >> q >> initial;
  soryuusi::BitSetSet<int> set(n);
  for (int i = 0; i < n; ++i) {
    if (initial[i] == '1') set.insert(i);
  }

  while (q--) {
    int type, k;
    soryuusi::cin >> type >> k;
    if (type == 0) {
      set.insert(k);
    } else if (type == 1) {
      set.erase(k);
    } else if (type == 2) {
      soryuusi::cout << set.contains(k) << '\n';
    } else if (type == 3) {
      const int answer = set.next(k);
      soryuusi::cout << (answer == n ? -1 : answer) << '\n';
    } else {
      soryuusi::cout << set.prev(k + 1) << '\n';
    }
  }
}
