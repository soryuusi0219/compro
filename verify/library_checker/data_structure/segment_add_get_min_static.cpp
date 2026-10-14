#include "library/utility/fastio.hpp"
#include <iostream>
#include <tuple>
#include <vector>

#include "library/data_structure/li_chao_tree_static.hpp"

using Segment = std::tuple<long long, long long, long long, long long>;

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<Segment> segments(n);
  for (auto& [left, right, a, b] : segments) soryuusi::cin >> left >> right >> a >> b;
  std::vector<int> types(q);
  std::vector<Segment> additions;
  std::vector<long long> coordinates;
  for (int i = 0; i < q; ++i) {
    soryuusi::cin >> types[i];
    if (types[i] == 0) {
      Segment segment;
      auto& [left, right, a, b] = segment;
      soryuusi::cin >> left >> right >> a >> b;
      additions.push_back(segment);
    } else {
      long long x;
      soryuusi::cin >> x;
      coordinates.push_back(x);
    }
  }
  if (coordinates.empty()) return 0;
  constexpr long long infinity = (1LL << 62);
  soryuusi::StaticLiChaoTree<long long, infinity> tree(coordinates);
  for (auto [left, right, a, b] : segments) tree.add_Segment({a, b}, left, right);
  int addition_index = 0, coordinate_index = 0;
  for (int type : types) {
    if (type == 0) {
      auto [left, right, a, b] = additions[addition_index++];
      tree.add_Segment({a, b}, left, right);
    } else {
      const long long answer = tree.get_min(coordinates[coordinate_index++]);
      if (answer == infinity) soryuusi::cout << "INFINITY\n";
      else soryuusi::cout << answer << '\n';
    }
  }
}
