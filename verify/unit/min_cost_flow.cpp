#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include <atcoder/mincostflow>

#include "library/graph/min_cost_flow.hpp"

namespace {

using Graph = soryuusi::MinCostFlow<int,int,long long>;
using Point = std::pair<int,long long>;
constexpr long long inf = std::numeric_limits<long long>::max();

struct Edge { int u,v,capacity,cost; };

std::vector<long long> brute(int n, const std::vector<Edge>& edges) {
  std::vector<int> balance(n);
  std::vector<long long> costs(1+2*edges.size(),inf);
  auto dfs = [&](auto&& self, int i, long long cost) -> void {
    if(i == int(edges.size())) {
      if(balance[0] < 0 || !std::all_of(balance.begin()+1,balance.end()-1,[](int d) { return d == 0; })) return;
      costs[balance[0]] = std::min(costs[balance[0]],cost);
      return;
    }
    const auto& [u,v,cap,c] = edges[i];
    for(int f = 0; f <= cap; ++f) {
      balance[u] += f;
      balance[v] -= f;
      self(self,i+1,cost+f*c);
      balance[u] -= f;
      balance[v] += f;
    }
  };
  dfs(dfs,0,0);
  while(costs.back() == inf) costs.pop_back();
  return costs;
}

void check_slope(const std::vector<Point>& slope, const std::vector<long long>& costs, int limit) {
  assert(slope.front() == Point(0,0));
  const int final_flow = std::min(int(costs.size())-1,limit);
  assert(slope.back() == Point(final_flow,costs[final_flow]));
  long long previous = std::numeric_limits<long long>::min();
  for(int i = 1; i < int(slope.size()); ++i) {
    const auto [l,cl] = slope[i-1];
    const auto [r,cr] = slope[i];
    assert(r > l && (cr-cl)%(r-l) == 0);
    const long long marginal = (cr-cl)/(r-l);
    assert(previous < marginal);
    previous = marginal;
    for(int f = l; f <= r; ++f) assert(costs[f] == cl+(f-l)*marginal);
  }
}

void random_cases() {
  std::mt19937 rng(402831);
  for(int rep = 0; rep < 1600; ++rep) {
    const int n = 2+rng()%4, m = rng()%9;
    Graph g(n);
    atcoder::mcf_graph<int,long long> acl(n);
    std::vector<int> p(n);
    for(int& x : p) x = int(rng()%11)-5;
    std::vector<Edge> original;
    for(int i = 0; i < m; ++i) {
      int u = rng()%n, v = rng()%n, cap = rng()%3, cost = rng()%8;
      if(rep%3 == 1) cost += p[v]-p[u];
      if(rep%3 == 2) {
        if(u > v) std::swap(u,v);
        if(u != v) cost -= 4;
      }
      original.push_back({u,v,cap,cost});
      assert(g.add_edge(u,v,cap,cost) == i);
      if(rep%3 == 0) acl.add_edge(u,v,cap,cost);
    }
    const auto costs = brute(n,original);
    const int limit = rng()%14;
    const int f = std::min(int(costs.size())-1,limit);
    check_slope(g.slope(0,n-1,limit),costs,limit);
    if(rep%3 == 0) check_slope(acl.slope(0,n-1,limit),costs,limit);
    assert(g.flow(0,n-1) == Point(int(costs.size())-1-f,costs.back()-costs[f]));
    assert(g.flow(0,n-1) == Point(0,0));
    const auto edges = g.edges();
    std::vector<int> balance(n);
    long long cost = 0;
    for(int i = 0; i < m; ++i) {
      const auto& [u,v,cap,flow,c] = edges[i];
      assert(u == original[i].u && v == original[i].v && cap == original[i].capacity && c == original[i].cost);
      assert(0 <= flow && flow <= cap && g.get_edge(i).flow == flow);
      balance[u] += flow;
      balance[v] -= flow;
      cost += flow*c;
    }
    for(int v = 0; v < n; ++v) assert(balance[v] == (v == 0 ? int(costs.size())-1 : v == n-1 ? 1-int(costs.size()) : 0));
    assert(cost == costs.back());
    g.reset();
    for(const auto& e : g.edges()) assert(e.flow == 0);
    check_slope(g.slope(0,n-1),costs,int(costs.size())-1);
  }
}

void boundary_cases() {
  Graph g(4);
  g.add_edge(0,1,3,-5);
  g.add_edge(1,3,3,2);
  g.add_edge(0,3,2,6);
  assert(g.flow(0,3,2) == Point(2,-6));
  assert(g.flow(1,0,1) == Point(1,5));
  assert(g.get_edge(0).flow == 1);
  g.reset();
  g.add_edge(2,2,1,-1);
  bool detected = false;
  try {
    g.flow(0,3);
  } catch(const std::domain_error&) {
    detected = true;
  }
  assert(detected && g.get_edge(0).flow == 0);

  Graph added(2);
  added.add_edge(0,1,1,4);
  assert(added.flow(0,1) == Point(1,4));
  added.add_edge(0,1,2,1);
  detected = false;
  try {
    added.flow(0,1);
  } catch(const std::domain_error&) {
    detected = true;
  }
  assert(detected);
  added.reset();
  assert(added.flow(0,1) == Point(3,6));

  const auto lo = std::numeric_limits<long long>::min(), hi = std::numeric_limits<long long>::max();
  soryuusi::MinCostFlow<long long,long long> wide(3);
  wide.add_edge(0,1,hi,lo);
  wide.add_edge(1,2,hi,lo);
  const auto [f,c] = wide.flow(0,2);
  assert(f == hi && c == __int128_t(hi)*lo*2);
  assert(wide.flow(2,0,1).second == -__int128_t(lo)*2);

  Graph equal(104);
  for(int i = 1; i <= 100; ++i) {
    equal.add_edge(0,i,1,-4);
    equal.add_edge(i,103,1,2);
    equal.add_edge(i,i,3,0);
  }
  equal.add_edge(0,101,3,0);
  equal.add_edge(101,103,3,0);
  equal.add_edge(0,102,2,3);
  equal.add_edge(102,103,2,2);
  assert(equal.slope(0,103,104) == (std::vector<Point>{{0,0},{100,-200},{103,-200},{104,-195}}));
  assert(equal.flow(0,103) == Point(1,5));

  constexpr int n = 100000;
  Graph chain(n);
  chain.reserve_edges(n-1);
  for(int i = n-1; i > 0; --i) chain.add_edge(i-1,i,1,-1);
  assert(chain.flow(0,n-1) == Point(1,1-n));
}

} // namespace

int main() {
  random_cases();
  boundary_cases();
  std::cout << "OK\n";
}
