// https://github.com/soryuusi0219/compro/blob/main/library/graph/extreme_vertex_set.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>

#include "../data_structure/dsu.hpp"
#include "internal/cut_contraction.hpp"
#include "undirected_cut_edge.hpp"

namespace soryuusi {

template <class Cap> struct ExtremeVertexSetTree {
  std::vector<int> parent;
  std::vector<Cap> cut;
  int root = -1;
  bool artificial_root = false;
};

namespace internal {

template <class Cap> struct ExtremeSetCandidates {
  std::vector<int> parent;
  std::vector<Cap> cut;
};

// Nagamochi's minimum-degree ordering yields a flat pair at the end of each round.
template <class Cap> ExtremeSetCandidates<Cap> extreme_set_candidates(int n, const std::vector<UndirectedCutEdge<Cap>>& edges) {
  assert(0 <= n && n <= std::numeric_limits<int>::max()/2);
  CutContraction<Cap> graph(n,edges);
  ExtremeSetCandidates<Cap> tree;
  tree.parent.assign(n,-1);
  tree.cut = graph.degree;
  if(n <= 1) return tree;
  tree.parent.reserve(2*n-1);
  tree.cut.reserve(2*n-1);

  DSU components(n);
  for(auto [u,v,w] : edges) if(w > 0 && u != v) components.merge(u,v);
  auto groups = components.groups();
  CutOrderingHeap<Cap,true> heap(n);
  std::vector<int> node(n), index(n);
  std::iota(node.begin(),node.end(),0);
  auto join = [&](int u, int v, Cap cut) {
    const int p = static_cast<int>(tree.parent.size());
    tree.parent[u] = tree.parent[v] = p;
    tree.parent.push_back(-1);
    tree.cut.push_back(cut);
    return p;
  };
  int root = -1;
  for(auto& active : groups) {
    for(int i = 0; i < static_cast<int>(active.size()); ++i) index[active[i]] = i;
    while(active.size() > 1) {
      heap.reset(active,graph.degree.data());
      while(heap.size() > 2) graph.for_each(heap.pop(),[&](int v, Cap w) { heap.improve(v,w); });
      const int u = heap.pop(), v = heap.pop();
      graph.contract(u,v);
      node[u] = join(node[u],node[v],graph.degree[u]);
      active[index[v]] = active.back();
      index[active.back()] = index[v];
      active.pop_back();
    }
    const int component_root = node[active[0]];
    root = root < 0 ? component_root : join(root,component_root,Cap(0));
  }
  return tree;
}

template <class Cap> ExtremeVertexSetTree<Cap> filter_extreme_set_candidates(int n, const ExtremeSetCandidates<Cap>& tree) {
  const int m = static_cast<int>(tree.parent.size());
  if(m == 0) return {};
  // Every violating subset contains an extreme subset of no greater cut.
  std::vector<Cap> proper_min(m,std::numeric_limits<Cap>::max());
  std::vector<int> id(m,-1);
  ExtremeVertexSetTree<Cap> result;
  result.parent.reserve(m);
  result.cut.reserve(m);
  for(int v = 0; v < m; ++v) {
    const int p = tree.parent[v];
    const bool extreme = v < n || tree.cut[v] < proper_min[v];
    if(p >= 0) proper_min[p] = std::min(proper_min[p],std::min(proper_min[v],tree.cut[v]));
    if(extreme || p < 0) {
      id[v] = static_cast<int>(result.parent.size());
      result.parent.push_back(-1);
      result.cut.push_back(tree.cut[v]);
      if(p < 0) result.artificial_root = !extreme;
    }
  }
  for(int v = m-2; v >= 0; --v) {
    const int p = id[tree.parent[v]];
    if(id[v] < 0) id[v] = p;
    else result.parent[id[v]] = p;
  }
  result.root = id.back();
  return result;
}

}  // namespace internal

template <class Cap> ExtremeVertexSetTree<Cap> extreme_vertex_set(int n, const std::vector<UndirectedCutEdge<Cap>>& edges) {
  static_assert(std::numeric_limits<Cap>::is_integer && !std::is_same_v<Cap,bool>);
  return internal::filter_extreme_set_candidates(n,internal::extreme_set_candidates(n,edges));
}

}  // namespace soryuusi
