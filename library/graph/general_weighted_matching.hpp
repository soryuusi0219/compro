// https://github.com/soryuusi0219/compro/blob/main/library/graph/general_weighted_matching.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <type_traits>
#include <vector>

#include "../data_structure/dsu.hpp"
#include "general_matching.hpp"
#include "internal/weighted_blossom.hpp"

namespace soryuusi {

enum class WeightedMatchingObjective { Weight, CardinalityThenWeight, Perfect };

template <class Weight = long long> class GeneralWeightedMatching {
  static_assert(std::numeric_limits<Weight>::is_integer && std::numeric_limits<Weight>::digits <= 64 && !std::is_same_v<Weight,bool>);

public:
  using TotalWeight = __int128_t;
  struct Edge { int u, v; Weight weight; };
  struct Result { int cardinality; TotalWeight weight; };

private:
  int n;
  bool solved = false;
  WeightedMatchingObjective last = WeightedMatchingObjective::Weight;
  std::optional<Result> result;
  std::vector<Edge> input;
  std::vector<int> mate, matched;

  // Each oriented tree edge is scanned once to build the order and once in DP.
  template <class Score> std::vector<int> tree_matching(int k, const std::vector<int>& ids, const std::vector<int>& local, Score score) const {
    struct Arc { int to, id; };
    std::vector<int> start(k+1);
    for(int id : ids) {
      ++start[local[input[id].u]+1];
      ++start[local[input[id].v]+1];
    }
    std::partial_sum(start.begin(),start.end(),start.begin());
    auto cursor = start;
    std::vector<Arc> adj(start.back());
    for(int id : ids) {
      const int u = local[input[id].u], v = local[input[id].v];
      adj[cursor[u]++] = {v,id};
      adj[cursor[v]++] = {u,id};
    }

    std::vector<int> parent(k,-1), order = {0}, choice(k,-1), partner(k,-1);
    order.reserve(k);
    for(int i = 0; i < k; ++i) {
      const int u = order[i];
      for(int j = start[u]; j < start[u+1]; ++j) {
        const int v = adj[j].to;
        if(v == parent[u]) continue;
        parent[v] = u;
        order.push_back(v);
      }
    }
    std::vector<TotalWeight> blocked(k), best(k);
    for(int i = k; i-- > 0;) {
      const int u = order[i];
      TotalWeight gain = 0;
      for(int j = start[u]; j < start[u+1]; ++j) {
        const auto [v,id] = adj[j];
        if(v == parent[u]) continue;
        blocked[u] += best[v];
        const TotalWeight extra = score(input[id].weight)+blocked[v]-best[v];
        if(extra > gain) {
          gain = extra;
          choice[u] = v;
        }
      }
      best[u] = blocked[u]+gain;
    }
    for(int u : order) if(partner[u] < 0 && choice[u] >= 0) {
      partner[u] = choice[u];
      partner[choice[u]] = u;
    }
    return partner;
  }

public:
  explicit GeneralWeightedMatching(int n = 0) : n(n), mate(n,-1), matched(n,-1) {
    assert(0 <= n && n <= (std::numeric_limits<int>::max()-1)/2);
  }

  int size() const { return n; }
  int edge_count() const { return static_cast<int>(input.size()); }
  void reserve_edges(int m) { input.reserve(m); }

  int add_edge(int u, int v, Weight weight) {
    assert(0 <= u && u < n && 0 <= v && v < n && input.size() < std::size_t(std::numeric_limits<int>::max()/2));
    input.push_back({u,v,weight});
    if(u != v) solved = false;
    return edge_count()-1;
  }

  Edge get_edge(int id) const { return input[id]; }
  int partner(int v) const { return mate[v]; }
  int matched_edge(int v) const { return matched[v]; }

  std::vector<int> matching_edges() const {
    std::vector<int> edges;
    if(result) edges.reserve(result->cardinality);
    for(int u = 0; u < n; ++u) if(mate[u] > u) edges.push_back(matched[u]);
    return edges;
  }

  std::optional<Result> solve(WeightedMatchingObjective objective = WeightedMatchingObjective::Weight) {
    if(solved && last == objective) return result;
    const bool perfect = objective == WeightedMatchingObjective::Perfect;
    const bool prioritize_size = objective != WeightedMatchingObjective::Weight;
    solved = false;
    last = objective;
    auto infeasible = [&]() -> std::optional<Result> {
      solved = true;
      return std::nullopt;
    };
    result.reset();
    std::fill(mate.begin(),mate.end(),-1);
    std::fill(matched.begin(),matched.end(),-1);
    if(perfect && (n&1)) return infeasible();

    DSU dsu(n);
    auto usable = [&](const Edge& e) { return e.u != e.v && (prioritize_size || e.weight > 0); };
    for(const auto& e : input) if(usable(e)) dsu.merge(e.u,e.v);
    auto groups = dsu.groups();
    std::vector<int> component(n), local(n);
    for(int g = 0; g < static_cast<int>(groups.size()); ++g) {
      const auto& vertices = groups[g];
      const int k = static_cast<int>(vertices.size());
      if(perfect && (k&1)) return infeasible();
      for(int i = 0; i < k; ++i) {
        component[vertices[i]] = g;
        local[vertices[i]] = i;
      }
    }

    std::vector<std::vector<int>> ids(groups.size());
    for(int id = 0; id < edge_count(); ++id) if(usable(input[id])) ids[component[input[id].u]].push_back(id);
    std::vector<int> selected;
    selected.reserve(n/2);
    for(int g = 0; g < static_cast<int>(groups.size()); ++g) if(!ids[g].empty()) {
      const int k = static_cast<int>(groups[g].size());
      Weight low = input[ids[g][0]].weight, high = low;
      for(int id : ids[g]) {
        low = std::min(low,input[id].weight);
        high = std::max(high,input[id].weight);
      }

      std::vector<int> partner;
      if(k == 2) {
        partner = {1,0};
      } else if(low == high) {
        GeneralMatching unweighted(k);
        unweighted.reserve_edges(static_cast<int>(ids[g].size()));
        for(int id : ids[g]) unweighted.add_edge(local[input[id].u],local[input[id].v]);
        unweighted.solve();
        partner.resize(k);
        for(int u = 0; u < k; ++u) partner[u] = unweighted.partner(u);
      } else {
        const TotalWeight range = TotalWeight(high)-low;
        const TotalWeight bonus = TotalWeight(k/2)*range+1;
        auto score = [&](Weight w) -> TotalWeight { return prioritize_size ? bonus+(TotalWeight(w)-low) : TotalWeight(w); };
        const TotalWeight maximum = score(high);
        const TotalWeight bound = TotalWeight(4)*(k+1)*maximum;
        auto run = [&]<class Dual>() {
          struct ScoredEdge { int u, v; Dual score; };
          std::vector<ScoredEdge> edges;
          edges.reserve(ids[g].size());
          for(int id : ids[g]) {
            const auto [u,v,w] = input[id];
            edges.push_back({local[u],local[v],Dual(score(w))});
          }
          return internal::WeightedBlossom<Dual>(k,edges,Dual(maximum)).solve();
        };
        if(ids[g].size() == std::size_t(k-1)) partner = tree_matching(k,ids[g],local,score);
        else if(bound <= std::numeric_limits<std::int32_t>::max()) partner = run.template operator()<std::int32_t>();
        else if(bound <= std::numeric_limits<std::int64_t>::max()) partner = run.template operator()<std::int64_t>();
        else partner = run.template operator()<TotalWeight>();
      }

      if(perfect && std::find(partner.begin(),partner.end(),-1) != partner.end()) return infeasible();
      std::vector<int> best(k,-1);
      for(int id : ids[g]) {
        const auto [u,v,w] = input[id];
        const int a = local[u], b = local[v], first = std::min(a,b);
        if(partner[a] == b && (best[first] < 0 || input[best[first]].weight < w)) best[first] = id;
      }
      for(int id : best) if(id >= 0) selected.push_back(id);
    }

    result = Result{static_cast<int>(selected.size()),0};
    for(int id : selected) {
      const auto [u,v,w] = input[id];
      mate[u] = v;
      mate[v] = u;
      matched[u] = matched[v] = id;
      result->weight += w;
    }
    solved = true;
    return result;
  }
};

}  // namespace soryuusi
