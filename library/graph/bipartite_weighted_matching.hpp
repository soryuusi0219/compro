// https://github.com/soryuusi0219/compro/blob/main/library/graph/bipartite_weighted_matching.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>
#include "bipartite_matching.hpp"
#include "min_cost_b_flow.hpp"
#include "project_selection.hpp"

namespace soryuusi {

template <class Weight = long long> class BipartiteWeightedMatching {
  static_assert(std::numeric_limits<Weight>::is_integer && std::numeric_limits<Weight>::is_signed && sizeof(Weight) <= 8);
  struct Edge { int left, right, capacity; Weight weight; };

  int l, r;
  std::vector<Edge> edges;

public:
  struct Result {
    __int128_t weight;
    long long size;
    std::vector<int> flow;
  };

  BipartiteWeightedMatching(int left, int right) : l(left), r(right) {
    if(l < 0 || r < 0 || l > std::numeric_limits<int>::max()-r-4) throw std::length_error("invalid bipartite sizes");
  }

  int add_edge(int left, int right, Weight weight, int capacity = 1) {
    assert(0 <= left && left < l && 0 <= right && right < r && capacity >= 0);
    if(edges.size() >= std::numeric_limits<int>::max()/2) throw std::length_error("too many weighted matching edges");
    const int id = int(edges.size());
    edges.push_back({left,right,capacity,weight});
    return id;
  }

  std::optional<Result> solve(std::span<const int> left_capacity = {}, std::span<const int> right_capacity = {}, long long required = -1) const {
    assert(left_capacity.empty() || left_capacity.size() == std::size_t(l));
    assert(right_capacity.empty() || right_capacity.size() == std::size_t(r));
    assert(required >= -1);
    const int s = l+r, t = s+1;
    if(edges.size()+std::size_t(s)+1 > std::numeric_limits<int>::max()/2) throw std::length_error("too many weighted matching arcs");
    MinCostBFlow<long long,Weight> graph(t+1);
    graph.reserve_edges(int(edges.size())+s+1);
    for(const auto& e : edges) graph.add_edge(l+e.right,e.left,-static_cast<long long>(e.capacity),0,e.weight);
    long long left_sum = 0, right_sum = 0;
    for(int u = 0; u < l; ++u) {
      const int cap = left_capacity.empty() ? 1 : left_capacity[u];
      assert(cap >= 0);
      graph.add_edge(s,u,0,cap,0);
      left_sum += cap;
    }
    for(int v = 0; v < r; ++v) {
      const int cap = right_capacity.empty() ? 1 : right_capacity[v];
      assert(cap >= 0);
      graph.add_edge(l+v,t,0,cap,0);
      right_sum += cap;
    }
    const long long limit = std::min(left_sum,right_sum);
    if(required > limit) return std::nullopt;
    graph.add_edge(t,s,required < 0 ? 0 : required,required < 0 ? limit : required,0);
    const auto answer = graph.solve();
    if(!answer) return std::nullopt;
    Result result{-answer->cost,answer->flow.back(),std::vector<int>(edges.size())};
    for(int id = 0; id < int(edges.size()); ++id) result.flow[id] = int(-answer->flow[id]);
    return result;
  }
};

struct WeightedBipartiteVertexSet {
  __int128_t weight;
  BipartiteMatching::VertexSet vertices;
};

template <class Weight> WeightedBipartiteVertexSet minimum_weight_bipartite_vertex_cover(const BipartiteMatching& graph, const std::vector<Weight>& left_weight, const std::vector<Weight>& right_weight) {
  const int l = graph.left_size(), r = graph.right_size();
  assert(left_weight.size() == std::size_t(l) && right_weight.size() == std::size_t(r));
  if(l > std::numeric_limits<int>::max()-r-2) throw std::length_error("too many bipartite vertices");
  ProjectSelection<Weight> model(l+r);
  for(int u = 0; u < l; ++u) model.add_cost(u,left_weight[u],false);
  for(int v = 0; v < r; ++v) model.add_cost(l+v,right_weight[v],true);
  for(const auto& [u,v] : graph.edges()) model.imply(u,l+v);
  const auto result = model.solve();
  WeightedBipartiteVertexSet cover{result->cost,{std::vector<bool>(l),std::vector<bool>(r)}};
  for(int u = 0; u < l; ++u) cover.vertices.left[u] = !result->selected[u];
  for(int v = 0; v < r; ++v) cover.vertices.right[v] = result->selected[l+v];
  return cover;
}

template <class Weight> WeightedBipartiteVertexSet maximum_weight_bipartite_independent_set(const BipartiteMatching& graph, const std::vector<Weight>& left_weight, const std::vector<Weight>& right_weight) {
  auto result = minimum_weight_bipartite_vertex_cover(graph,left_weight,right_weight);
  result.weight = -result.weight;
  for(Weight w : left_weight) result.weight += w;
  for(Weight w : right_weight) result.weight += w;
  result.vertices.left.flip();
  result.vertices.right.flip();
  return result;
}

} // namespace soryuusi
