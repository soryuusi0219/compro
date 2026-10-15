#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/segtree_template.hpp"

using namespace std;

int main() {

  int n, q;
  soryuusi::cin >> n >> q;

  std::vector<long long> a(n);
  for (int i = 0; i < n; ++i) soryuusi::cin >> a[i];
  soryuusi::SegtreeSum<long long> seg(a);

  while (q--) {
    int type, first;
    long long second;
    soryuusi::cin >> type >> first >> second;
    if (type == 0) {
      seg.set(first, seg.get(first) + second);
    } else {
      soryuusi::cout << seg.prod(first, static_cast<int>(second)) << '\n';
    }
  }
}
