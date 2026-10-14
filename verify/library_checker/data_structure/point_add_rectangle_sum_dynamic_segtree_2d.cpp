#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/dynamic_segtree_2d.hpp"

long long add(long long left, long long right) { return left + right; }
long long zero() { return 0; }

int main() {
  constexpr int coordinate_bound = 1000000010;
  soryuusi::Dynamic_Segtree2D<long long, add, zero> tree(coordinate_bound, coordinate_bound);
  int n, q;
  soryuusi::cin >> n >> q;
  while (n--) {
    int x, y;
    long long weight;
    soryuusi::cin >> x >> y >> weight;
    tree.add(x, y, weight);
  }
  while (q--) {
    int type;
    soryuusi::cin >> type;
    if (type == 0) {
      int x, y;
      long long weight;
      soryuusi::cin >> x >> y >> weight;
      tree.add(x, y, weight);
    } else {
      int left, down, right, up;
      soryuusi::cin >> left >> down >> right >> up;
      soryuusi::cout << tree.prod(left, right, down, up) << '\n';
    }
  }
}
