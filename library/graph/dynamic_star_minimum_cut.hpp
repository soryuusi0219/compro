// https://github.com/soryuusi0219/compro/blob/main/library/graph/dynamic_star_minimum_cut.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <vector>

#include "../tree/static_top_tree.hpp"
#include "extreme_vertex_set.hpp"
#include "undirected_cut_edge.hpp"

namespace soryuusi {

template <class Cap = long long> class DynamicStarMinimumCut {
  static_assert(std::numeric_limits<Cap>::is_integer && !std::is_same_v<Cap,bool>);

  struct Cluster {
    Cap sum, path_min, side_min;
  };

  static Cluster rake(Cluster x, Cluster y) {
    return {Cap(x.sum+y.sum),x.path_min,std::min({x.side_min,y.path_min,y.side_min})};
  }

  static Cluster compress(Cluster x, Cluster y) {
    return {Cap(x.sum+y.sum),std::min(Cap(x.path_min+y.sum),y.path_min),std::min(x.side_min,y.side_min)};
  }

  std::vector<Cap> leaf_cut;
  StaticTopTree<Cluster,rake,compress> tree;

public:
  DynamicStarMinimumCut(const std::vector<Cap>& weights, const std::vector<UndirectedCutEdge<Cap>>& edges) {
    assert(!weights.empty() && weights.size() <= std::size_t(std::numeric_limits<int>::max()/4));
    const int n = static_cast<int>(weights.size());
    auto candidate = internal::extreme_set_candidates(n,edges);
    const int m = static_cast<int>(candidate.parent.size());
    leaf_cut.assign(candidate.cut.begin(),candidate.cut.begin()+n);
    std::vector<Cluster> data(m);
    for(int v = 0; v < m; ++v) {
      const Cap w = v < n ? weights[v] : 0;
      assert(w >= 0);
      data[v] = {w,Cap(candidate.cut[v]+w),std::numeric_limits<Cap>::max()};
    }
    // Each node has at most one light child. StaticTopTree rakes it with
    // the heavy child, so their minima are independent until compression.
    tree = StaticTopTree<Cluster,rake,compress>(data);
    for(int v = 0; v < m-1; ++v) tree.add_edge(v,candidate.parent[v]);
    tree.build(m-1);
  }

  int size() const { return static_cast<int>(leaf_cut.size()); }

  Cap set(int v, Cap weight) {
    assert(0 <= v && v < size() && weight >= 0);
    tree.set(v,{weight,Cap(leaf_cut[v]+weight),std::numeric_limits<Cap>::max()});
    return minimum_cut();
  }

  Cap minimum_cut() const {
    const auto cluster = tree.get();
    return std::min(cluster.path_min,cluster.side_min);
  }
};

}  // namespace soryuusi
