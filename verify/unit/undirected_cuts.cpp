#include <algorithm>
#include <cassert>
#include <limits>
#include <random>
#include <vector>

#include "library/graph/dynamic_star_minimum_cut.hpp"
#include "library/graph/extreme_vertex_set.hpp"
#include "library/graph/global_minimum_cut.hpp"
#include "library/graph/gomory_hu_tree.hpp"
#include "library/graph/max_flow.hpp"

namespace {

using namespace soryuusi;
using Cap = long long;
using Edge = UndirectedCutEdge<Cap>;

template <class Mask> Cap cut_weight(const std::vector<Edge>& edges, Mask in_side) {
  Cap total = 0;
  for(auto [u,v,w] : edges) if(in_side(u) != in_side(v)) total += w;
  return total;
}

void check(int n, const std::vector<Edge>& edges, std::mt19937& rng) {
  const int all = (1<<n)-1;
  const Cap inf = std::numeric_limits<Cap>::max();
  std::vector<Cap> cuts(all+1);
  for(int mask = 1; mask <= all; ++mask) cuts[mask] = cut_weight(edges,[&](int v) { return (mask>>v)&1; });

  for(auto method : {GlobalMinCutMethod::Auto,GlobalMinCutMethod::Dense,GlobalMinCutMethod::Sparse}) {
    auto result = global_minimum_cut(n,edges,method);
    if(n < 2) {
      assert(!result);
      continue;
    }
    assert(result && result->value == *std::min_element(cuts.begin()+1,cuts.end()-1));
    const auto& side = result->side;
    assert(side.size() == std::size_t(n));
    assert(std::find(side.begin(),side.end(),true) != side.end());
    assert(std::find(side.begin(),side.end(),false) != side.end());
    assert(cut_weight(edges,[&](int v) { return side[v]; }) == result->value);
  }

  MaxFlow<Cap> direct(n);
  for(auto [u,v,w] : edges) {
    direct.add_edge(u,v,w);
    direct.add_edge(v,u,w);
  }
  std::vector<std::vector<Cap>> pair_cut(n,std::vector<Cap>(n,inf));
  for(int u = 0; u < n; ++u) for(int v = 0; v < u; ++v) {
    for(int mask = 1; mask < all; ++mask) if(((mask>>v)^(mask>>u))&1) pair_cut[u][v] = std::min(pair_cut[u][v],cuts[mask]);
    direct.reset();
    assert(direct.flow(u,v) == pair_cut[u][v]);
  }
  for(auto method : {GomoryHuFlow::Dinic,GomoryHuFlow::PushRelabel}) {
    GomoryHuTree<Cap> tree(n,edges,method);
    assert(tree.size() == n);
    if(n) assert(tree.parent(0) == -1);
    for(int u = 1; u < n; ++u) {
      const int p = tree.parent(u);
      assert(0 <= p && p < n && p != u);
      const auto side = tree.cut_side(u);
      assert(side[u] && !side[p]);
      assert(cut_weight(edges,[&](int v) { return side[v]; }) == tree.capacity(u));
      for(int v = 0; v < u; ++v) {
        assert(tree.min_cut_value(u,v) == pair_cut[u][v]);
        const int pivot = tree.min_cut_edge(u,v);
        const auto witness = tree.cut_side(pivot);
        assert(witness[u] != witness[v] && tree.capacity(pivot) == pair_cut[u][v]);
      }
    }
  }

  const auto tree = extreme_vertex_set(n,edges);
  if(n == 0) {
    assert(tree.root == -1 && tree.parent.empty() && tree.cut.empty() && !tree.artificial_root);
    return;
  }
  const int m = static_cast<int>(tree.parent.size());
  assert(m >= n && m <= 2*n-1);
  assert(tree.root == m-1 && tree.parent.back() == -1);
  std::vector<int> mask(m);
  for(int v = 0; v < n; ++v) mask[v] = 1<<v;
  std::vector<bool> extreme(all+1,true);
  extreme[0] = false;
  for(int x = 1; x <= all; ++x) {
    for(int y = (x-1)&x; y; y = (y-1)&x) if(cuts[y] <= cuts[x]) extreme[x] = false;
  }
  assert(tree.artificial_root == !extreme[all]);
  std::vector<bool> represented(all+1,false);
  for(int v = 0; v < m; ++v) {
    const int p = tree.parent[v], x = mask[v];
    assert(x != 0 && tree.cut[v] == cuts[x]);
    if(v != tree.root || !tree.artificial_root) {
      assert(!represented[x]);
      represented[x] = true;
    }
    if(p >= 0) {
      assert(v < p && p < m && !(mask[p]&x));
      mask[p] |= x;
    }
    for(int y = (x-1)&x; y && v != tree.root; y = (y-1)&x) {
      if(extreme[y]) assert(cuts[y] > cuts[x]);
    }
  }
  assert(mask.back() == all && represented == extreme);

  std::vector<Cap> weights(n);
  for(auto& w : weights) w = rng()%15;
  DynamicStarMinimumCut<Cap> dynamic(weights,edges);
  assert(dynamic.size() == n);
  auto naive = [&] {
    Cap best = inf;
    for(int x = 1; x <= all; ++x) {
      Cap sum = cuts[x];
      for(int v = 0; v < n; ++v) if((x>>v)&1) sum += weights[v];
      best = std::min(best,sum);
    }
    return best;
  };
  assert(dynamic.minimum_cut() == naive());
  for(int i = 0; i < 30; ++i) {
    const int v = rng()%n;
    weights[v] = i%3 == 0 ? 0 : rng()%43;
    assert(dynamic.set(v,weights[v]) == naive());
    assert(dynamic.minimum_cut() == naive());
  }
}

void wide_cuts() {
  using Wide = __int128_t;
  const Wide w = Wide(std::numeric_limits<Cap>::max())+19;
  const std::vector<UndirectedCutEdge<Wide>> edges = {{0,1,w},{0,1,w},{1,2,w},{0,2,w}};
  assert(global_minimum_cut(3,edges)->value == 2*w);
  const GomoryHuTree<Wide> tree(3,edges);
  assert(tree.min_cut_value(0,1) == 3*w);
  assert(tree.min_cut_value(0,2) == 2*w);
  DynamicStarMinimumCut<Wide> dynamic(std::vector<Wide>(3,w),edges);
  assert(dynamic.minimum_cut() == 3*w);
  assert(dynamic.set(0,0) == 2*w);
  // An attained maximum is a valid capacity as well.
  const Cap max = std::numeric_limits<Cap>::max();
  const std::vector<Edge> single = {{0,1,max}};
  assert(global_minimum_cut(2,single)->value == max);
  assert(GomoryHuTree<Cap>(2,single).min_cut_value(0,1) == max);
  assert(extreme_vertex_set(2,single).cut == (std::vector<Cap>{max,max,0}));
}

}  // namespace

int main() {
  std::mt19937 rng(827331);
  wide_cuts();
  check(0,{},rng);
  check(1,{{0,0,9}},rng);
  check(8,{},rng);
  check(7,{{0,1,9},{1,2,90},{2,3,9},{4,5,8},{5,6,0}},rng);
  std::vector<Edge> complete, path, star;
  for(int u = 0; u < 9; ++u) for(int v = 0; v < u; ++v) complete.push_back({u,v,1});
  for(int u = 1; u < 9; ++u) {
    path.push_back({u-1,u,Cap(1)<<u});
    star.push_back({0,u,1});
    star.push_back({u,0,1});
  }
  check(9,complete,rng);
  check(9,path,rng);
  check(9,star,rng);
  for(int repeat = 0; repeat < 500; ++repeat) {
    const int n = 1+rng()%9, m = rng()%(n*n+1);
    std::vector<Edge> edges;
    for(int i = 0; i < m; ++i) edges.push_back({int(rng()%n),int(rng()%n),Cap(rng()%8)});
    check(n,edges,rng);
  }
}
