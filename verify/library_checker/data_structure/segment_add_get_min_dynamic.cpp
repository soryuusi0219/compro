#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/li_chao_tree_dynamic.hpp"

int main() {
  constexpr long long bound = (1LL << 30);
  constexpr long long infinity = (1LL << 62);
  soryuusi::DynamicLiChaoTree<long long, -bound, bound, 1, infinity> tree;
  int n, q;
  soryuusi::cin >> n >> q;
  while (n--) {
    long long left, right, a, b;
    soryuusi::cin >> left >> right >> a >> b;
    tree.add_Segment({a, b}, left, right);
  }
  while (q--) {
    int type;
    soryuusi::cin >> type;
    if (type == 0) {
      long long left, right, a, b;
      soryuusi::cin >> left >> right >> a >> b;
      tree.add_Segment({a, b}, left, right);
    } else {
      long long x;
      soryuusi::cin >> x;
      const long long answer = tree.get_min(x);
      if (answer == infinity) soryuusi::cout << "INFINITY\n";
      else soryuusi::cout << answer << '\n';
    }
  }
}
