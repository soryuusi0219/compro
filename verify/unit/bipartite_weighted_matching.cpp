#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <vector>

#include "library/graph/bipartite_weighted_matching.hpp"

namespace {

using Wide = __int128_t;
using Weighted = soryuusi::BipartiteWeightedMatching<>;

void random_b_matching() {
  std::mt19937 rng(98572);
  for(int rep = 0; rep < 900; ++rep) {
    const int l = 1+rng()%4, r = 1+rng()%4, m = rng()%9;
    Weighted graph(l,r);
    struct Edge { int u,v,w,cap; };
    std::vector<Edge> edges;
    std::vector<int> a(l), b(r);
    for(int& cap : a) cap = rng()%3;
    for(int& cap : b) cap = rng()%3;
    for(int i = 0; i < m; ++i) {
      const int u = rng()%l, v = rng()%r, w = int(rng()%19)-9, cap = rng()%3;
      assert(graph.add_edge(u,v,w,cap) == i);
      edges.push_back({u,v,w,cap});
    }
    std::vector<std::optional<Wide>> best(2*l+1);
    auto dfs = [&](auto&& self, int i, int count, Wide weight) -> void {
      if(i == m) {
        if(!best[count] || weight > best[count]) best[count] = weight;
        return;
      }
      const auto [u,v,w,cap] = edges[i];
      const int limit = std::min({a[u],b[v],cap});
      for(int f = 0; f <= limit; ++f) {
        a[u] -= f;
        b[v] -= f;
        self(self,i+1,count+f,weight+w*f);
        a[u] += f;
        b[v] += f;
      }
    };
    dfs(dfs,0,0,0);
    for(int k : {-1,int(rng()%best.size())}) {
      const auto result = graph.solve(a,b,k);
      const auto expected = k < 0 ? *std::max_element(best.begin(),best.end()) : best[k];
      assert(bool(result) == bool(expected));
      if(!result) continue;
      assert(result->weight == expected && (k < 0 || result->size == k));
      const auto& flow = result->flow;
      auto left = a, right = b;
      int count = 0;
      Wide value = 0;
      for(int i = 0; i < m; ++i) {
        const auto [u,v,w,cap] = edges[i];
        assert(0 <= flow[i] && flow[i] <= cap);
        assert((left[u] -= flow[i]) >= 0 && (right[v] -= flow[i]) >= 0);
        value += w*flow[i];
        count += flow[i];
      }
      assert(result->size == count && result->weight == value);
    }
  }
}

void weighted_cover() {
  std::mt19937 rng(93414);
  for(int rep = 0; rep < 600; ++rep) {
    const int l = rng()%5, r = rng()%5;
    soryuusi::BipartiteMatching graph(l,r);
    std::vector<int> a(l), b(r);
    for(int& w : a) w = int(rng()%35)-17;
    for(int& w : b) w = int(rng()%35)-17;
    for(int u = 0; u < l; ++u) for(int v = 0; v < r; ++v) if(rng()%2) graph.add_edge(u,v);
    Wide min = 200, max = -200;
    for(int mask = 0; mask < (1<<(l+r)); ++mask) {
      int sum = 0;
      for(int u = 0; u < l; ++u) if(mask>>u&1) sum += a[u];
      for(int v = 0; v < r; ++v) if(mask>>(l+v)&1) sum += b[v];
      bool cover = true, independent = true;
      for(auto [u,v] : graph.edges()) {
        cover &= (mask>>u&1) || (mask>>(l+v)&1);
        independent &= !(mask>>u&1) || !(mask>>(l+v)&1);
      }
      if(cover) min = std::min(min,Wide(sum));
      if(independent) max = std::max(max,Wide(sum));
    }
    const auto c = soryuusi::minimum_weight_bipartite_vertex_cover(graph,a,b);
    const auto d = soryuusi::maximum_weight_bipartite_independent_set(graph,a,b);
    assert(c.weight == min && d.weight == max);
    for(auto [u,v] : graph.edges()) assert((c.vertices.left[u] || c.vertices.right[v]) && !(d.vertices.left[u] && d.vertices.right[v]));
    int wc = 0, wd = 0;
    for(int u = 0; u < l; ++u) wc += c.vertices.left[u]*a[u], wd += d.vertices.left[u]*a[u];
    for(int v = 0; v < r; ++v) wc += c.vertices.right[v]*b[v], wd += d.vertices.right[v]*b[v];
    assert(wc == min && wd == max);
  }
}

void boundaries() {
  constexpr auto lo = std::numeric_limits<long long>::min();
  Weighted graph(1,1);
  graph.add_edge(0,0,lo);
  assert(graph.solve()->weight == 0);
  assert(graph.solve({},{},1)->weight == lo);
  assert(!graph.solve({},{},2));
  Weighted wide(1,1);
  constexpr auto hi = std::numeric_limits<long long>::max();
  wide.add_edge(0,0,hi,4);
  const std::vector<int> bound{4};
  assert(wide.solve(bound,bound)->weight == Wide(hi)*4);
}

} // namespace

int main() {
  random_b_matching();
  weighted_cover();
  boundaries();
  std::cout << "OK\n";
}
