#include "library/utility/fastio.hpp"
#include <iostream>
#include <tuple>
#include <vector>

#include "library/data_structure/li_chao_tree_static.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<std::pair<long long, long long>> initial(n);
  for (auto& [a, b] : initial) soryuusi::cin >> a >> b;
  std::vector<std::tuple<int, long long, long long>> queries(q);
  std::vector<long long> coordinates;
  for (auto& [type, a, b] : queries) {
    soryuusi::cin >> type >> a;
    if (type == 0) soryuusi::cin >> b;
    else coordinates.push_back(a);
  }
  if (coordinates.empty()) return 0;
  constexpr long long infinity = (1LL << 62);
  soryuusi::StaticLiChaoTree<long long, infinity> tree(coordinates);
  for (auto [a, b] : initial) tree.add_Line({a, b});
  for (auto [type, a, b] : queries) {
    if (type == 0) tree.add_Line({a, b});
    else soryuusi::cout << tree.get_min(a) << '\n';
  }
}
