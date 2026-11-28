// https://github.com/soryuusi0219/compro/blob/main/library/graph/internal/selection_cut.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../max_flow.hpp"

namespace soryuusi::internal {

template <class Cost> struct SelectionCut {
  static_assert(std::numeric_limits<Cost>::is_integer && std::numeric_limits<Cost>::is_signed && sizeof(Cost) <= 8);
  using Wide = __int128_t;
  using Narrow = std::uint64_t;
  static constexpr int source = 0, sink = 1;
  static constexpr int edge_limit = std::numeric_limits<int>::max()/2;

  struct Arc {
    int from, to;
    Narrow cap;
  };
  Wide constant = 0;
  Wide finite_capacity = 0;
  std::vector<Wide> unary;
  std::vector<Arc> arcs;
  std::vector<std::pair<int,Wide>> wide_arcs;
  std::vector<std::pair<int,int>> hard_arcs;
  bool forced_true = false, forced_false = false;

  explicit SelectionCut(int n) : unary(n) {}

  int add_vertex() {
    if(unary.size() >= std::numeric_limits<int>::max()-2) throw std::length_error("too many selection vertices");
    const int id = int(unary.size())+2;
    unary.push_back(0);
    return id;
  }

  void reserve_edges(int m) {
    if(m < 0 || m > edge_limit) throw std::length_error("too many selection edges");
    arcs.reserve(m);
  }

  bool hard_conflict() const {
    std::vector<int> first(unary.size()+2,-1), next(hard_arcs.size());
    for(int i = 0; i < int(hard_arcs.size()); ++i) {
      const int u = hard_arcs[i].first;
      next[i] = first[u];
      first[u] = i;
    }
    std::vector<int> work{source};
    std::vector<bool> seen(unary.size()+2);
    seen[source] = true;
    for(int i = 0; i < int(work.size()); ++i) {
      for(int e = first[work[i]]; e != -1; e = next[e]) {
        const int v = hard_arcs[e].second;
        if(v == sink) return true;
        if(seen[v]) continue;
        seen[v] = true;
        work.push_back(v);
      }
    }
    return false;
  }

  void add_arc(int u, int v, Wide cap) {
    if(cap == 0 || u == v) return;
    assert(cap > 0);
    if(arcs.size()+hard_arcs.size() == edge_limit) throw std::length_error("too many selection edges");
    if(cap > std::numeric_limits<Narrow>::max()) {
      wide_arcs.emplace_back(int(arcs.size()),cap);
      try {
        arcs.push_back({u,v,0});
      } catch(...) {
        wide_arcs.pop_back();
        throw;
      }
    } else {
      arcs.push_back({u,v,Narrow(cap)});
    }
    finite_capacity += cap;
  }

  void imply(int u, int v) {
    if(u == v) return;
    if(arcs.size()+hard_arcs.size() == edge_limit) throw std::length_error("too many selection edges");
    hard_arcs.emplace_back(u,v);
    forced_true |= u == source;
    forced_false |= v == sink;
  }

  void add_delta(int v, Wide delta) { unary[v-2] += delta; }

  std::optional<std::pair<Wide,std::vector<bool>>> solve(bool push_relabel) const {
    if(forced_true && forced_false && hard_conflict()) return std::nullopt;
    Wide cut_bound = finite_capacity, bias = constant;
    int unary_count = 0;
    for(Wide d : unary) {
      if(d < 0) bias += d, cut_bound -= d;
      else cut_bound += d;
      unary_count += d != 0;
    }
    const int model_edges = int(arcs.size()+hard_arcs.size());
    if(model_edges == 0) {
      std::vector<bool> selected(unary.size()+2);
      selected[source] = true;
      for(int i = 0; i < int(unary.size()); ++i) selected[i+2] = unary[i] < 0;
      return std::pair{bias,std::move(selected)};
    }
    if(unary_count > edge_limit-model_edges) throw std::length_error("too many selection edges");
    const Wide hard = cut_bound+1;
    auto run = [&]<class Cap>() -> std::optional<std::pair<Wide,std::vector<bool>>> {
      MaxFlow<Cap> graph(int(unary.size())+2);
      graph.reserve_edges(model_edges+unary_count);
      for(int i = 0; i < int(unary.size()); ++i) {
        const Wide d = unary[i];
        if(d < 0) graph.add_edge(source,i+2,Cap(-d));
        if(d > 0) graph.add_edge(i+2,sink,Cap(d));
      }
      std::size_t j = 0;
      for(int i = 0; i < int(arcs.size()); ++i) {
        const auto [u,v,c] = arcs[i];
        Wide cap = c;
        if constexpr(sizeof(Cap) > sizeof(Narrow)) {
          if(j < wide_arcs.size() && wide_arcs[j].first == i) cap = wide_arcs[j++].second;
        }
        graph.add_edge(u,v,Cap(cap));
      }
      for(const auto& [u,v] : hard_arcs) graph.add_edge(u,v,Cap(hard));
      const Cap flow = push_relabel ? graph.flow_push_relabel(source,sink) : graph.flow(source,sink,Cap(hard));
      if(flow >= hard) return std::nullopt;
      return std::pair{bias+flow,graph.min_cut(source)};
    };
    if(hard <= std::numeric_limits<Narrow>::max()) return run.template operator()<Narrow>();
    return run.template operator()<Wide>();
  }
};

} // namespace soryuusi::internal
