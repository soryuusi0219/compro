#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <vector>

#include "library/graph/min_cost_b_flow.hpp"

namespace {

using Wide = __int128_t;
using Graph = soryuusi::MinCostBFlow<>;
using ll = long long;

struct Edge { int u,v; ll lower,upper,cost; };

std::optional<Wide> brute(const std::vector<Edge>& edges, std::vector<ll> remaining) {
  std::optional<Wide> best;
  auto dfs = [&](auto&& self, int i, Wide sum) -> void {
    if(i == int(edges.size())) {
      if(std::all_of(remaining.begin(),remaining.end(),[](ll b) { return b == 0; }) && (!best || sum < *best)) best = sum;
      return;
    }
    const auto& [u,v,lower,upper,cost] = edges[i];
    for(ll f = lower; f <= upper; ++f) {
      remaining[u] -= f;
      remaining[v] += f;
      self(self,i+1,sum+Wide(f)*cost);
      remaining[u] += f;
      remaining[v] -= f;
    }
  };
  dfs(dfs,0,0);
  return best;
}

void check(const Graph::Result& answer, const std::vector<Edge>& edges, const std::vector<ll>& supply) {
  assert(answer.flow.size() == edges.size() && answer.potential.size() == supply.size());
  std::vector<Wide> balance(supply.begin(),supply.end());
  Wide cost = 0;
  for(int i = 0; i < int(edges.size()); ++i) {
    const auto& [u,v,lo,hi,c] = edges[i];
    const ll f = answer.flow[i];
    assert(lo <= f && f <= hi);
    balance[u] -= f;
    balance[v] += f;
    cost += Wide(c)*f;
    const Wide reduced = c+answer.potential[u]-answer.potential[v];
    if(f < hi) assert(reduced >= 0);
    if(f > lo) assert(reduced <= 0);
  }
  assert(cost == answer.cost);
  assert(std::all_of(balance.begin(),balance.end(),[](Wide b) { return b == 0; }));
}

void random_cases() {
  std::mt19937 rng(90931);
  for(int rep = 0; rep < 2800; ++rep) {
    const int n = 1+rng()%5, m = rng()%8;
    Graph graph(n);
    std::vector<Edge> edges;
    std::vector<ll> supply(n);
    for(int i = 0; i < m; ++i) {
      const int u = rng()%n, v = rng()%n;
      const ll lo = int(rng()%7)-3, hi = lo+int(rng()%4), c = int(rng()%15)-7;
      const ll f = lo+int(rng()%(hi-lo+1));
      supply[u] += f;
      supply[v] -= f;
      assert(graph.add_edge(u,v,lo,hi,c) == i);
      edges.push_back({u,v,lo,hi,c});
    }
    for(int i = 0; i < 3; ++i) {
      const auto expected = brute(edges,supply);
      const auto answer = i == 2 ? graph.solve() : graph.solve(supply);
      assert(bool(answer) == bool(expected));
      if(answer) {
        check(*answer,edges,supply);
        assert(answer->cost == *expected);
      }
      if(i == 0) {
        ++supply[rng()%n];
        if(rep%5 != 0) --supply[rng()%n];
      } else {
        std::fill(supply.begin(),supply.end(),0);
      }
    }
  }
}

void sparse_certificates() {
  std::mt19937 rng(208190);
  constexpr int n = 320;
  for(int rep = 0; rep < 8; ++rep) {
    Graph graph(n);
    std::vector<Edge> edges;
    const std::vector<ll> supply(n,0);
    for(int i = 0; i < 3*n; ++i) {
      const int u = rng()%n, v = rng()%n;
      const ll width = rep%2 ? 1LL<<54 : 1000;
      const ll lo = -ll(rng()%width), hi = rng()%width, c = int(rng()%201)-100;
      graph.add_edge(u,v,lo,hi,c);
      edges.push_back({u,v,lo,hi,c});
    }
    const auto answer = graph.solve(supply);
    assert(answer);
    check(*answer,edges,supply);
  }
}

void boundaries() {
  const auto empty = Graph(0).solve();
  assert(empty && empty->cost == 0 && empty->flow.empty() && empty->potential.empty());

  constexpr ll lo = std::numeric_limits<ll>::min(), hi = std::numeric_limits<ll>::max();
  {
    Graph graph(5);
    std::vector<Edge> edges{{0,1,lo,hi,lo},{1,0,0,hi,1},{2,3,1,hi,7},{3,2,1,hi,-8},{4,4,lo,1,-1}};
    for(auto [u,v,l,r,c] : edges) graph.add_edge(u,v,l,r,c);
    auto answer = graph.solve();
    assert(answer && (answer->flow == std::vector<ll>{hi,hi,hi,hi,1}));
    check(*answer,edges,std::vector<ll>(5,0));
    assert(!graph.solve({1,-1,0,1,-1}));
  }
  {
    Graph graph(4);
    std::vector<Edge> edges{{0,1,lo,hi,hi},{2,3,lo,hi,lo}};
    for(auto [u,v,l,r,c] : edges) graph.add_edge(u,v,l,r,c);
    const std::vector<ll> supply{hi,-hi,hi,-hi};
    auto answer = graph.solve(supply);
    assert(answer && answer->cost == -hi);
    check(*answer,edges,supply);
  }
  {
    constexpr int n = 600;
    Graph graph(n);
    std::vector<Edge> edges;
    for(int i = 0; i < n; ++i) {
      edges.push_back({i,(i+1)%n,0,1000000000000LL,i == n-1 ? -n : 1});
      const auto e = edges.back();
      graph.add_edge(e.u,e.v,e.lower,e.upper,e.cost);
    }
    auto answer = graph.solve();
    assert(answer && answer->cost == -1000000000000LL);
    check(*answer,edges,std::vector<ll>(n));
  }
}

} // namespace

int main() {
  random_cases();
  boundaries();
  sparse_certificates();
  std::cout << "OK\n";
}
