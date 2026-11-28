// https://github.com/soryuusi0219/compro/blob/main/library/graph/global_minimum_cut.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "../data_structure/dsu.hpp"
#include "internal/cut_contraction.hpp"
#include "undirected_cut_edge.hpp"

namespace soryuusi {

enum class GlobalMinCutMethod { Auto, Dense, Sparse };

template <class Cap> struct GlobalMinCutResult {
  Cap value;
  std::vector<bool> side;
};

template <class Cap> std::optional<GlobalMinCutResult<Cap>> global_minimum_cut(int n, const std::vector<UndirectedCutEdge<Cap>>& edges, GlobalMinCutMethod method = GlobalMinCutMethod::Auto) {
  static_assert(std::numeric_limits<Cap>::is_integer && !std::is_same_v<Cap,bool>);
  assert(0 <= n && n <= std::numeric_limits<int>::max()/2);
  if(n < 2) return std::nullopt;
  DSU components(n);
  Cap least_edge = std::numeric_limits<Cap>::max();
  std::size_t positive_edges = 0;
  for(auto [u,v,w] : edges) {
    assert(0 <= u && u < n && 0 <= v && v < n && w >= 0);
    if(u == v || w == 0) continue;
    components.merge(u,v);
    least_edge = std::min(least_edge,w);
    ++positive_edges;
  }
  if(components.size(0) < n) {
    GlobalMinCutResult<Cap> result{0,std::vector<bool>(n)};
    const int root = components.leader(0);
    for(int v = 0; v < n; ++v) result.side[v] = components.leader(v) == root;
    return result;
  }

  std::vector<int> active(n), node(n), parent(n,-1);
  parent.reserve(2*n-1);
  std::iota(active.begin(),active.end(),0);
  std::iota(node.begin(),node.end(),0);
  auto run = [&](auto&& phase, auto&& contract) {
    Cap best = 0;
    int best_node = -1;
    while(active.size() > 1) {
      const auto [s,t,cut] = phase();
      if(best_node < 0 || cut < best) {
        best = cut;
        best_node = node[t];
        if(best == least_edge) break;
      }
      contract(s,t);
      const int p = static_cast<int>(parent.size());
      parent[node[s]] = parent[node[t]] = p;
      parent.push_back(-1);
      node[s] = p;
      *std::find(active.begin(),active.end(),t) = active.back();
      active.pop_back();
    }
    std::vector<bool> side(parent.size());
    side[best_node] = true;
    for(int i = best_node-1; i >= 0; --i) if(parent[i] >= 0) side[i] = side[parent[i]];
    side.resize(n);
    return GlobalMinCutResult<Cap>{best,std::move(side)};
  };

  if(method == GlobalMinCutMethod::Auto && n <= 1024) {
    const std::size_t threshold = std::max<std::size_t>(1,std::size_t(n)*n/20);
    if(positive_edges >= threshold) {
      std::vector<bool> adjacent(std::size_t(n)*n);
      std::size_t distinct = 0;
      for(auto [u,v,w] : edges) if(w > 0 && u != v) {
        if(u > v) std::swap(u,v);
        const std::size_t pair = std::size_t(u)*n+v;
        if(adjacent[pair]) continue;
        adjacent[pair] = true;
        if(++distinct == threshold) {
          method = GlobalMinCutMethod::Dense;
          break;
        }
      }
    }
  }

  if(method == GlobalMinCutMethod::Dense) {
    std::vector<Cap> weight(std::size_t(n)*n), key(n);
    std::vector<int> remaining;
    remaining.reserve(n);
    auto row = [&](int v) { return weight.data()+std::size_t(v)*n; };
    for(auto [u,v,w] : edges) if(u != v) {
      row(u)[v] += w;
      row(v)[u] += w;
    }
    return run([&] {
      remaining = active;
      std::fill(key.begin(),key.end(),Cap(0));
      int s = -1;
      while(remaining.size() > 1) {
        const auto pick = std::max_element(remaining.begin(),remaining.end(),[&](int u, int v) { return key[u] < key[v]; });
        s = *pick;
        *pick = remaining.back();
        remaining.pop_back();
        const Cap* adjacent = row(s);
        for(int v : remaining) key[v] += adjacent[v];
      }
      const int t = remaining[0];
      return std::tuple{s,t,key[t]};
    },[&](int s, int t) {
      Cap* dst = row(s);
      const Cap* src = row(t);
      for(int v : active) if(v != s && v != t) row(v)[s] = dst[v] += src[v];
    });
  }
  internal::CutContraction<Cap> graph(n,edges);
  internal::CutOrderingHeap<Cap,false> heap(n);
  return run([&] {
    heap.reset(active);
    int s = -1;
    while(heap.size() > 1) {
      s = heap.pop();
      graph.for_each(s,[&](int v, Cap w) { heap.improve(v,w); });
    }
    const int t = heap.top();
    return std::tuple{s,t,heap.value(t)};
  },[&](int s, int t) { graph.contract(s,t); });
}

}  // namespace soryuusi
