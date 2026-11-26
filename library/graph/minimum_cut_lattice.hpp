// https://github.com/soryuusi0219/compro/blob/main/library/graph/minimum_cut_lattice.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <utility>
#include <vector>
#include "max_flow.hpp"
#include "scc.hpp"
#include "static_graph.hpp"

namespace soryuusi {

struct MinimumCutLattice {
  enum class Side : unsigned char { free, source, sink };

  std::vector<int> component;
  static_graph<> dag;
  std::vector<Side> side;

  std::vector<bool> minimum_source_side() const {
    std::vector<bool> cut(component.size());
    for(int v = 0; v < int(component.size()); ++v) cut[v] = side[component[v]] == Side::source;
    return cut;
  }

  std::vector<bool> maximum_source_side() const {
    std::vector<bool> cut(component.size());
    for(int v = 0; v < int(component.size()); ++v) cut[v] = side[component[v]] != Side::sink;
    return cut;
  }
};

template <class Cap> std::optional<MinimumCutLattice> minimum_cut_lattice(const MaxFlow<Cap>& graph, int s, int t) {
  const int n = graph.size();
  assert(0 <= s && s < n && 0 <= t && t < n && s != t && n < std::numeric_limits<int>::max());
  int residual = 0;
  graph.for_each_residual_arc([&](int u, int v, Cap) { residual += u != v; });
  auto [groups,component] = [&] {
    SCC scc(n,residual);
    graph.for_each_residual_arc([&](int u, int v, Cap) { if(u != v) scc.add_edge(u,v); });
    return scc.scc_ids();
  }();
  const int source = component[s], sink = component[t];
  if(source == sink) return std::nullopt;

  static_graph<> dag(groups);
  dag.start.assign(std::size_t(groups)+1,0);
  graph.for_each_residual_arc([&](int u, int v, Cap) {
    const int a = component[u], b = component[v];
    if(a != b) ++dag.start[a+1];
  });
  std::partial_sum(dag.start.begin(),dag.start.end(),dag.start.begin());
  dag.es.resize(dag.start.back());
  graph.for_each_residual_arc([&](int u, int v, Cap) {
    const int a = component[u], b = component[v];
    if(a != b) dag.es[dag.start[a]++] = {b};
  });
  std::move_backward(dag.start.begin(),dag.start.end()-1,dag.start.end());
  dag.start[0] = 0;

  // Compact parallel arcs in place using one stamp per destination.
  std::vector<int> last(groups,-1);
  int count = 0;
  for(int a = 0; a < groups; ++a) {
    const int first = dag.start[a];
    dag.start[a] = count;
    for(int i = first; i < dag.start[a+1]; ++i) {
      const int b = dag.es[i].to;
      if(last[b] == a) continue;
      last[b] = a;
      dag.es[count++] = {b};
    }
  }
  dag.start[groups] = count;
  dag.es.resize(count);
  if(dag.es.capacity()/2 > dag.es.size()) std::vector(dag.es).swap(dag.es);
  dag.initialized = true;

  using Side = MinimumCutLattice::Side;
  std::vector<Side> side(groups,Side::free);
  side[source] = Side::source;
  for(int a = source; a < groups; ++a) if(side[a] == Side::source) {
    for(int b : dag.vertices(a)) side[b] = Side::source;
  }
  if(side[sink] == Side::source) return std::nullopt;
  side[sink] = Side::sink;
  for(int a = sink-1; a >= 0; --a) {
    for(int b : dag.vertices(a)) if(side[b] == Side::sink) {
      side[a] = Side::sink;
      break;
    }
  }
  return MinimumCutLattice{std::move(component),std::move(dag),std::move(side)};
}

} // namespace soryuusi
