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
    long long a, b;
    soryuusi::cin >> a >> b;
    tree.add_Line({a, b});
  }
  while (q--) {
    int type;
    long long a, b;
    soryuusi::cin >> type >> a;
    if (type == 0) {
      soryuusi::cin >> b;
      tree.add_Line({a, b});
    } else {
      soryuusi::cout << tree.get_min(a) << '\n';
    }
  }
}
