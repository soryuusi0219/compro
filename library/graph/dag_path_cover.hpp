// https://github.com/soryuusi0219/compro/blob/main/library/graph/dag_path_cover.hpp
#pragma once
#include <cassert>
#include <limits>
#include <span>
#include <utility>
#include <vector>
#include "bipartite_matching.hpp"

namespace soryuusi {

inline std::vector<std::vector<int>> dag_path_cover(int n, std::span<const std::pair<int,int>> edges) {
  assert(edges.size() <= std::numeric_limits<int>::max());
  BipartiteMatching graph(n,n);
  graph.reserve_edges(int(edges.size()));
  for(auto [u,v] : edges) graph.add_edge(u,v);
  const int matched = graph.solve();
  std::vector<std::vector<int>> paths;
  paths.reserve(n-matched);
  for(int v = 0; v < n; ++v) if(graph.right_partner(v) < 0) {
    auto& path = paths.emplace_back();
    for(int u = v; u >= 0; u = graph.left_partner(u)) path.push_back(u);
  }
  return paths;
}

} // namespace soryuusi
