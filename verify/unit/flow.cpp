#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include <atcoder/maxflow>

#include "library/graph/bounded_flow.hpp"
#include "library/graph/flow_decomposition.hpp"
#include "library/graph/max_flow.hpp"

namespace {

using soryuusi::BoundedFlow;
using soryuusi::FlowEdge;
using soryuusi::MaxFlow;
using soryuusi::decompose_flow;

template <class Cap> void check_decomposition(int n, const std::vector<FlowEdge<Cap>>& edges, int s = -1, int t = -1) {
  const auto components = decompose_flow(n,edges,s,t);
  std::vector<__int128_t> restored(edges.size());
  std::vector<int> seen(n,-1);
  for(int i = 0; i < int(components.size()); ++i) {
    const auto& [amount, ids, cycle] = components[i];
    assert(amount > 0 && !ids.empty());
    const int from = edges[ids.front()].from;
    int v = from;
    for(int id : ids) {
      assert(0 <= id && id < int(edges.size()) && edges[id].from == v && seen[v] != i);
      seen[v] = i;
      restored[id] += amount;
      v = edges[id].to;
    }
    if(cycle) assert(v == from);
    else assert(from == s && v == t && seen[t] != i);
  }
  for(int i = 0; i < int(edges.size()); ++i) assert(restored[i] == edges[i].flow);
}

void max_flow_random(std::mt19937& rng) {
  for(int rep = 0; rep < 2400; ++rep) {
    const int n = 2+rng()%7, m = rng()%35;
    MaxFlow<long long> g(n);
    atcoder::mf_graph<long long> acl(n);
    std::vector<FlowEdge<long long>> original;
    for(int i = 0; i < m; ++i) {
      const int u = rng()%n, v = rng()%n;
      const long long cap = rng()%9;
      assert(g.add_edge(u,v,cap) == i);
      acl.add_edge(u,v,cap);
      original.push_back({u,v,cap,0});
    }
    auto optimum = std::numeric_limits<long long>::max();
    for(int mask = 1; mask < (1<<(n-1)); mask += 2) {
      long long cut = 0;
      for(const auto& e : original) if((mask>>e.from&1) && !(mask>>e.to&1)) cut += e.cap;
      optimum = std::min(optimum,cut);
    }
    assert(acl.flow(0,n-1) == optimum);
    const long long limit = rng()%15;
    assert(g.flow(0,n-1,limit) == std::min(optimum,limit));
    assert(g.min_cut(0)[n-1] == (limit < optimum));
    const auto extra = optimum-std::min(optimum,limit);
    assert((rep%2 == 0 ? g.flow(0,n-1) : g.flow_push_relabel(0,n-1)) == extra);
    assert(g.flow(0,n-1) == 0);
    const auto cut = g.min_cut(0);
    assert(cut[0] && !cut[n-1]);
    auto edges = g.edges();
    std::vector<long long> balance(n);
    long long cut_value = 0;
    for(int i = 0; i < m; ++i) {
      const auto& [u,v,c,f] = edges[i];
      assert(u == original[i].from && v == original[i].to && c == original[i].cap && 0 <= f && f <= c);
      assert(g.get_edge(i).flow == f);
      if(cut[u] && !cut[v]) cut_value += c;
      balance[u] += f;
      balance[v] -= f;
    }
    for(int v = 0; v < n; ++v) assert(balance[v] == (v == 0 ? optimum : v == n-1 ? -optimum : 0));
    assert(cut_value == optimum);
    check_decomposition(n,edges,0,n-1);
    g.reset();
    for(int i = 0; i < m; ++i) assert(g.get_edge(i).cap == original[i].cap && g.get_edge(i).flow == 0);
    assert(g.flow_push_relabel(0,n-1) == optimum);
    check_decomposition(n,g.edges(),0,n-1);
  }
}

void push_relabel_stress(std::mt19937& rng) {
  for(int rep = 0; rep < 160; ++rep) {
    const int n = 12+rng()%240, m = n*(3+rng()%12);
    MaxFlow<long long> g(n);
    atcoder::mf_graph<long long> acl(n);
    for(int i = 0; i < m; ++i) {
      int u = rng()%n, v = rng()%n;
      long long c = rng()%10000;
      if(rep%2 == 0 && i < n-2) {
        u = 0;
        v = i+1;
        c = 1000000;
      }
      g.add_edge(u,v,c);
      acl.add_edge(u,v,c);
    }
    const long long expected = acl.flow(0,n-1);
    assert(g.flow_push_relabel(0,n-1) == expected);
    assert(!g.min_cut(0)[n-1]);
    check_decomposition(n,g.edges(),0,n-1);
  }
  // Many distinct distances to t: one exit saturates in each Dinic phase.
  const int n = 2000, t = n-1;
  MaxFlow<int> ladder(n);
  for(int v = 0; v < t; ++v) {
    if(v+1 != t) ladder.add_edge(v,v+1,t);
    ladder.add_edge(v,t,1);
  }
  assert(ladder.flow_push_relabel(0,t) == t);
  assert(!ladder.min_cut(0)[t]);
  check_decomposition(n,ladder.edges(),0,t);
}

void max_flow_boundaries() {
  MaxFlow<long long> g(3);
  constexpr long long cap = std::numeric_limits<long long>::max();
  int a = g.add_edge(0,1,cap), b = g.add_edge(1,2,cap);
  g.add_edge(1,0,cap);
  const int loop = g.add_edge(1,1,cap);
  g.add_edge(0,2,0);
  assert(g.flow(0,2,0) == 0 && g.min_cut(0)[2]);
  assert(g.flow(0,2,cap-10) == cap-10);
  assert(g.flow(0,2) == 10 && !g.min_cut(0)[2]);
  assert(g.get_edge(loop).flow == 0);
  g.change_edge(a,cap-1,cap-1);
  g.change_edge(b,cap-1,cap-1);
  assert(g.flow(0,2) == 0);
  g.reset();
  assert(g.flow(0,2) == cap-1);
  assert(g.add_edge(0,2,2) == 5);
  assert(g.flow(0,2) == 2);
  check_decomposition(3,g.edges(),0,2);
  g.reset();
  assert(g.flow_push_relabel(0,2) == cap && g.min_cut(0)[2]);
  assert(g.flow(0,2) == 1 && !g.min_cut(0)[2]);

  const auto umax = std::numeric_limits<unsigned long long>::max();
  check_decomposition(2,std::vector<FlowEdge<unsigned long long>>{{0,1,umax,umax},{0,1,umax,umax}},0,1);

  MaxFlow<unsigned short> small(2);
  small.add_edge(0,1,2);
  assert(small.flow(0,1) == 2 && small.get_edge(0).cap == 2);

  const int n = 300000;
  MaxFlow<int> chain(n);
  chain.reserve_edges(n-1);
  assert(chain.flow(0,n-1) == 0);
  for(int i = 1; i < n; ++i) chain.add_edge(i-1,i,2);
  assert(chain.flow(0,n-1,1) == 1);
  assert(chain.flow(0,n-1) == 1);
  assert(!chain.min_cut(0)[n-1]);
  chain.reset();
  assert(chain.flow_push_relabel(0,n-1) == 2);
}

struct Bounds {
  int from, to, lower, upper;
};

bool brute_bounds(const std::vector<Bounds>& edges, std::vector<int>& supply, int i = 0) {
  if(i == int(edges.size())) return std::all_of(supply.begin(),supply.end(),[](int b) { return b == 0; });
  const auto& [u,v,lo,hi] = edges[i];
  for(int f = lo; f <= hi; ++f) {
    supply[u] -= f;
    supply[v] += f;
    const bool feasible = brute_bounds(edges,supply,i+1);
    supply[u] += f;
    supply[v] -= f;
    if(feasible) return true;
  }
  return false;
}

void check_bounds(const std::vector<Bounds>& edges, const std::vector<int>& supply, const std::vector<int>& flow) {
  assert(flow.size() == edges.size());
  std::vector<int> balance(supply.size());
  for(int i = 0; i < int(edges.size()); ++i) {
    const auto& [u,v,lo,hi] = edges[i];
    assert(lo <= flow[i] && flow[i] <= hi);
    balance[u] += flow[i];
    balance[v] -= flow[i];
  }
  assert(balance == supply);
}

void bounded_flow_random(std::mt19937& rng) {
  for(int rep = 0; rep < 2400; ++rep) {
    const int n = 2+rng()%4, m = rng()%8;
    BoundedFlow<int> g(n);
    std::vector<Bounds> edges;
    std::vector<int> supply(n);
    for(int i = 0; i < m; ++i) {
      int u = rng()%n, v = rng()%n, lo = int(rng()%5)-2, hi = lo+rng()%4;
      int f = lo+rng()%(hi-lo+1);
      assert(g.add_edge(u,v,lo,hi) == i);
      edges.push_back({u,v,lo,hi});
      supply[u] += f;
      supply[v] -= f;
    }
    for(int test = 0; test < 3; ++test) {
      const auto result = test == 2 ? g.solve() : g.solve(supply);
      assert(bool(result) == brute_bounds(edges,supply));
      if(result) check_bounds(edges,supply,*result);
      if(test == 0) ++supply[rng()%n], --supply[rng()%n];
      else std::fill(supply.begin(),supply.end(),0);
    }
    const int lo = int(rng()%7)-3, hi = lo+rng()%5;
    bool feasible = false;
    for(int value = lo; value <= hi && !feasible; ++value) {
      supply[0] = value;
      supply[n-1] = -value;
      feasible = brute_bounds(edges,supply);
    }
    const auto bounded = g.solve(0,n-1,lo,hi);
    assert(bool(bounded) == feasible);
    if(bounded) {
      const auto& [value,flow] = *bounded;
      assert(lo <= value && value <= hi);
      supply[0] = value;
      supply[n-1] = -value;
      check_bounds(edges,supply,flow);
    }
  }
}

void bounded_flow_wide() {
  constexpr long long lo = std::numeric_limits<long long>::min();
  constexpr long long hi = std::numeric_limits<long long>::max();
  BoundedFlow<long long> g(4);
  g.add_edge(0,1,lo,hi);
  g.add_edge(2,3,0,hi);
  g.add_edge(2,2,lo,lo);
  const auto result = g.solve({hi,-hi,hi,-hi});
  assert(result && (*result == std::vector<long long>{hi,hi,lo}));
  assert(g.solve({0,0,0,1}) == std::nullopt);
  BoundedFlow<long long> large_span(2);
  large_span.add_edge(0,1,lo,hi);
  large_span.add_edge(1,0,lo,lo);
  const auto narrow_residual = large_span.solve({1,-1});
  assert(narrow_residual && (*narrow_residual == std::vector<long long>{lo+1,lo}));
  BoundedFlow<long long> fixed(2);
  fixed.add_edge(0,1,lo,lo);
  fixed.add_edge(0,1,hi,hi);
  assert(!fixed.solve());
  assert(fixed.solve({-1,1}));
  assert(BoundedFlow<int>(0).solve()->empty());
}

void decomposition_cycles(std::mt19937& rng) {
  for(int rep = 0; rep < 1200; ++rep) {
    const int n = 2+rng()%15;
    std::vector<FlowEdge<long long>> edges;
    for(int i = 0; i < 10; ++i) {
      const long long amount = 1+rng()%7;
      const int begin = rng()%n;
      int u = begin;
      for(int j = 0; j < i; ++j) {
        int v = rng()%n;
        edges.push_back({u,v,amount+1,amount});
        u = v;
      }
      edges.push_back({u,begin,amount+1,amount});
    }
    std::shuffle(edges.begin(),edges.end(),rng);
    check_decomposition(n,edges);
    edges.push_back({0,n-1,2,2});
    check_decomposition(n,edges,0,n-1);
  }
}

} // namespace

int main() {
  std::mt19937 rng(872401);
  max_flow_random(rng);
  max_flow_boundaries();
  push_relabel_stress(rng);
  bounded_flow_random(rng);
  bounded_flow_wide();
  decomposition_cycles(rng);
  std::cout << "OK\n";
}
