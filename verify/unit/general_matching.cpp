#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include "library/graph/bipartite_matching.hpp"
#include "library/graph/bipartite_weighted_matching.hpp"
#include "library/graph/general_matching.hpp"
#include "library/graph/general_weighted_matching.hpp"
#include "library/math/internal/internal_bit.hpp"

namespace {

using namespace soryuusi;
using Weight = std::int64_t;
using Wide = __int128_t;
using Edge = GeneralWeightedMatching<Weight>::Edge;
using Objective = WeightedMatchingObjective;

struct Score { int cardinality = 0; Wide weight = 0; };

Score naive(int n, const std::vector<Edge>& edges, bool by_size) {
  const Wide absent = -(Wide(1)<<100);
  std::vector<std::vector<Wide>> weight(n,std::vector<Wide>(n,absent));
  for(auto [u,v,w] : edges) if(u != v && weight[u][v] < w) weight[u][v] = weight[v][u] = w;
  std::vector<Score> dp(1u<<n);
  for(unsigned mask = 1; mask < dp.size(); ++mask) {
    const int u = soryuusi::countr_zero(mask);
    const unsigned others = mask&(mask-1);
    dp[mask] = dp[others];
    for(unsigned bits = others; bits; bits &= bits-1) {
      const int v = soryuusi::countr_zero(bits);
      if(weight[u][v] == absent) continue;
      auto score = dp[others^(1u<<v)];
      ++score.cardinality;
      score.weight += weight[u][v];
      const auto best = dp[mask];
      if(by_size && score.cardinality != best.cardinality ? score.cardinality > best.cardinality : score.weight > best.weight) dp[mask] = score;
    }
  }
  return dp.back();
}

template <class Graph> Score matching(const Graph& graph, const std::vector<Edge>& edges) {
  Score score;
  std::vector<bool> used(graph.size());
  for(int id : graph.matching_edges()) {
    assert(0 <= id && id < int(edges.size()));
    const auto [u,v,w] = edges[id];
    assert(u != v && !used[u] && !used[v]);
    assert(graph.partner(u) == v && graph.partner(v) == u);
    assert(graph.matched_edge(u) == id && graph.matched_edge(v) == id);
    used[u] = used[v] = true;
    ++score.cardinality;
    score.weight += w;
  }
  for(int u = 0; u < graph.size(); ++u) if(!used[u]) {
    assert(graph.partner(u) == -1 && graph.matched_edge(u) == -1);
  }
  return score;
}

void check(int n, const std::vector<Edge>& edges) {
  GeneralMatching plain(n);
  GeneralWeightedMatching<Weight> weighted(n);
  for(int id = 0; id < int(edges.size()); ++id) {
    const auto [u,v,w] = edges[id];
    assert(plain.add_edge(u,v) == id);
    assert(weighted.add_edge(u,v,w) == id);
    const auto a = plain.get_edge(id);
    const auto b = weighted.get_edge(id);
    assert(a.u == u && a.v == v && b.u == u && b.v == v && b.weight == w);
  }
  const auto max_size = naive(n,edges,true);
  const auto max_weight = naive(n,edges,false);
  assert(plain.solve() == max_size.cardinality && plain.solve() == max_size.cardinality);
  assert(matching(plain,edges).cardinality == max_size.cardinality);
  for(auto objective : {Objective::Weight,Objective::Perfect,Objective::CardinalityThenWeight,Objective::Weight}) {
    const auto result = weighted.solve(objective);
    const auto same = weighted.solve(objective);
    const auto actual = matching(weighted,edges);
    if(objective == Objective::Perfect && 2*max_size.cardinality != n) {
      assert(!result && !same && !actual.cardinality);
    } else {
      assert(result && same);
      assert(result->weight == same->weight && result->cardinality == same->cardinality);
      assert(result->weight == actual.weight && result->cardinality == actual.cardinality);
      if(objective == Objective::Weight) assert(result->weight == max_weight.weight);
      else assert(result->weight == max_size.weight && result->cardinality == max_size.cardinality);
    }
  }
}

void compare_bipartite(std::mt19937& rng) {
  for(int rep = 0; rep < 80; ++rep) {
    const int l = 1+rng()%18, r = 1+rng()%18;
    BipartiteMatching reference(l,r);
    BipartiteWeightedMatching<Weight> costs(l,r);
    GeneralMatching plain(l+r);
    GeneralWeightedMatching<Weight> weighted(l+r);
    const int m = rng()%(l*r+1);
    for(int j = 0; j < m; ++j) {
      const int u = rng()%l, v = rng()%r;
      const Weight w = int(rng()%151)-70;
      reference.add_edge(u,v);
      costs.add_edge(u,v,w);
      plain.add_edge(u,l+v);
      weighted.add_edge(u,l+v,w);
    }
    const int cardinality = reference.solve();
    assert(plain.solve() == cardinality);
    assert(weighted.solve()->weight == costs.solve()->weight);
    const auto lex = weighted.solve(Objective::CardinalityThenWeight);
    assert(lex->cardinality == cardinality && lex->weight == costs.solve({}, {},cardinality)->weight);
  }
}

void repeat_updates() {
  GeneralMatching plain(4);
  GeneralWeightedMatching<Weight> weighted(4);
  plain.add_edge(0,1);
  weighted.add_edge(0,1,100);
  assert(plain.solve() == 1 && weighted.solve()->weight == 100);
  assert(!weighted.solve(Objective::Perfect));
  assert(weighted.matching_edges().empty() && weighted.partner(0) == -1);
  plain.add_edge(0,2);
  plain.add_edge(1,3);
  weighted.add_edge(0,2,0);
  weighted.add_edge(1,3,-5);
  assert(plain.solve() == 2);
  assert(weighted.solve()->weight == 100);
  assert(weighted.solve(Objective::Perfect)->weight == -5);

  GeneralWeightedMatching<std::uint64_t> wide(4);
  const auto max = std::numeric_limits<std::uint64_t>::max();
  wide.add_edge(0,1,max);
  wide.add_edge(2,3,max);
  wide.add_edge(0,3,1);
  assert(wide.solve()->weight == Wide(2)*max);
  assert(wide.solve(Objective::Perfect)->weight == Wide(2)*max);
}

}  // namespace

int main() {
  const Weight low = std::numeric_limits<Weight>::min(), high = std::numeric_limits<Weight>::max();
  check(0,{});
  check(1,{{0,0,high}});
  check(4,{{0,1,100},{0,2,0},{1,3,-1},{3,3,high}});
  check(4,{{0,1,low},{2,3,low},{1,2,high},{2,1,high-1}});
  check(4,{{0,1,high},{2,3,high},{1,2,1}});
  check(6,{{1,0,-1},{0,1,9},{1,0,9},{2,3,0},{4,4,1}});
  check(6,{{0,1,3},{0,2,2},{0,3,1},{0,4,1},{0,5,1}});
  check(14,{{0,1,1},{2,3,1},{4,5,1},{6,7,1},{8,9,1},{10,11,1},{1,3,1},{7,9,1},{0,13,1},{6,12,1},{1,2,1},{3,4,1},{0,6,1},{7,8,1},{9,10,1},{5,13,1},{11,12,1}});
  std::vector<Edge> nested;
  for(int v = 1; v+1 < 10; v += 2) {
    nested.push_back({v-1,v,v+30});
    nested.push_back({v,v+1,2*v});
    nested.push_back({v+1,v-1,v+17});
  }
  nested.push_back({9,0,1});
  check(10,nested);
  std::mt19937 rng(871504);
  for(int rep = 0; rep < 1600; ++rep) {
    const int n = 1+rng()%11, m = rng()%(n*n+1);
    std::vector<Edge> edges;
    for(int i = 0; i < m; ++i) {
      Weight w = int(rng()%41)-20;
      if(rep%9 == 0) w *= high/31;
      if(rep%7 == 0) w = w%3;
      edges.push_back({int(rng()%n),int(rng()%n),w});
    }
    check(n,edges);
  }
  for(int rep = 0; rep < 120; ++rep) {
    const int n = 2+rng()%10;
    std::vector<Edge> tree;
    for(int v = 1; v < n; ++v) tree.push_back({v,int(rng()%v),Weight(rng()%101)-50});
    check(n,tree);
  }
  compare_bipartite(rng);
  repeat_updates();
}
