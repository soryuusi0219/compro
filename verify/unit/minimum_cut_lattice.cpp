#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <tuple>
#include <vector>

#include "library/graph/max_flow.hpp"
#include "library/graph/minimum_cut_lattice.hpp"

namespace {

using Side = soryuusi::MinimumCutLattice::Side;

void random_cuts() {
  std::mt19937 rng(781900);
  for(int rep = 0; rep < 2500; ++rep) {
    const int n = 2+rng()%7, m = rng()%36, s = rng()%n, t = (s+1+rng()%(n-1))%n;
    soryuusi::MaxFlow<int> g(n);
    for(int i = 0; i < m; ++i) {
      const int u = rng()%n, v = rng()%n, c = rng()%8;
      g.add_edge(u,v,c);
    }
    const int value = rep%2 == 0 ? g.flow(s,t) : g.flow_push_relabel(s,t);
    const auto result = soryuusi::minimum_cut_lattice(g,s,t);
    assert(result);
    const auto& [component,dag,side] = *result;
    assert(dag.size() == int(side.size()) && g.size() == n);
    const auto edges = g.edges();
    std::vector<std::tuple<int,int,int>> residual, scanned;
    std::vector<std::vector<bool>> pair(dag.size(),std::vector<bool>(dag.size()));
    for(const auto& [u,v,c,f] : edges) {
      if(c > f) residual.emplace_back(u,v,c-f);
      if(f > 0) residual.emplace_back(v,u,f);
    }
    g.for_each_residual_arc([&](int u, int v, int cap) {
      scanned.emplace_back(u,v,cap);
      if(component[u] != component[v]) pair[component[u]][component[v]] = true;
    });
    assert(residual == scanned);
    for(int u = 0; u < dag.size(); ++u) {
      for(int v : dag.vertices(u)) {
        assert(u < v && pair[u][v]);
        pair[u][v] = false;
      }
      assert(std::none_of(pair[u].begin(),pair[u].end(),[](bool found) { return found; }));
    }
    int smallest = (1<<n)-1, largest = 0;
    for(int mask = 0; mask < (1<<n); ++mask) {
      if(!(mask>>s&1) || (mask>>t&1)) continue;
      int capacity = 0;
      for(const auto& [u,v,c,f] : edges) if((mask>>u&1) && !(mask>>v&1)) capacity += c;
      assert(capacity >= value);
      std::vector<int> closed(dag.size(),-1);
      bool admissible = true;
      for(int v = 0; v < n; ++v) {
        const int selected = mask>>v&1, id = component[v];
        if(closed[id] >= 0 && closed[id] != selected) admissible = false;
        closed[id] = selected;
      }
      for(int u = 0; u < dag.size(); ++u) for(int v : dag.vertices(u)) if(closed[u] && !closed[v]) admissible = false;
      assert(admissible == (capacity == value));
      if(admissible) smallest &= mask, largest |= mask;
    }
    const auto minimum = result->minimum_source_side(), maximum = result->maximum_source_side();
    for(int v = 0; v < n; ++v) {
      assert(minimum[v] == bool(smallest>>v&1) && maximum[v] == bool(largest>>v&1));
      assert(side[component[v]] == (minimum[v] ? Side::source : maximum[v] ? Side::free : Side::sink));
    }
  }
}

void boundaries() {
  soryuusi::MaxFlow<long long> g(4);
  const auto max = std::numeric_limits<long long>::max();
  int id = g.add_edge(0,3,max);
  g.add_edge(0,3,max);
  assert(!soryuusi::minimum_cut_lattice(g,0,3));
  assert(g.flow(0,3) == max && !soryuusi::minimum_cut_lattice(g,0,3));
  assert(g.flow(0,3) == max);
  const auto done = soryuusi::minimum_cut_lattice(g,0,3);
  assert(done && done->dag.edge_size() == 1);
  g.change_edge(id,max,0);
  assert(!soryuusi::minimum_cut_lattice(g,0,3));
  assert(done->minimum_source_side() == (std::vector<bool>{true,false,false,false}));
  assert(done->maximum_source_side() == (std::vector<bool>{true,true,true,false}));

  const int n = 160000;
  soryuusi::MaxFlow<int> long_chain(n);
  long_chain.reserve_edges(n+30000);
  for(int i = 1; i+2 < n; ++i) long_chain.add_edge(i,i+1,1);
  for(int i = 0; i < 30000; ++i) long_chain.add_edge(1,2,1);
  const auto chain = soryuusi::minimum_cut_lattice(long_chain,0,n-1);
  assert(chain && chain->dag.size() == n && chain->dag.edge_size() == n-3);
  assert(chain->side[chain->component[1]] == Side::free);
}

} // namespace

int main() {
  random_cuts();
  boundaries();
  std::cout << "OK\n";
}
