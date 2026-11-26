#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <utility>
#include <vector>

#include "library/graph/bipartite_matching.hpp"
#include "library/graph/dag_path_cover.hpp"
#include "library/graph/dulmage_mendelsohn_decomposition.hpp"
#include "library/graph/max_flow.hpp"

namespace {

using Graph = soryuusi::BipartiteMatching;

struct Oracle {
  int size = -1, count = 0, allowed = 0, essential = 0;
};

Oracle enumerate(const Graph& graph) {
  std::vector<std::vector<int>> incident(graph.left_size());
  for(int e = 0; e < graph.edge_count(); ++e) incident[graph.get_edge(e).first].push_back(e);
  Oracle best;
  auto dfs = [&](auto&& self, int u, int used, int chosen, int size) -> void {
    if(u == graph.left_size()) {
      if(size > best.size) best = {size,1,chosen,chosen};
      else if(size == best.size) {
        ++best.count;
        best.allowed |= chosen;
        best.essential &= chosen;
      }
      return;
    }
    self(self,u+1,used,chosen,size);
    for(int e : incident[u]) {
      const int v = graph.get_edge(e).second;
      if(!(used>>v&1)) self(self,u+1,used|(1<<v),chosen|(1<<e),size+1);
    }
  };
  dfs(dfs,0,0,0,0);
  return best;
}

void check(Graph& graph) {
  const auto best = enumerate(graph);
  assert(graph.solve() == best.size && graph.solve() == best.size);
  const auto matching = graph.matching_edges();
  assert(matching.size() == std::size_t(best.size));
  const auto cover = graph.minimum_vertex_cover();
  const auto independent = graph.maximum_independent_set();
  int covered = 0;
  for(int u = 0; u < graph.left_size(); ++u) {
    covered += cover.left[u];
    assert(cover.left[u] != independent.left[u]);
    const int v = graph.left_partner(u);
    if(v >= 0) {
      assert(graph.right_partner(v) == u);
      assert(graph.left_match_edges()[u] == graph.right_match_edge(v));
    }
  }
  for(int v = 0; v < graph.right_size(); ++v) {
    covered += cover.right[v];
    assert(cover.right[v] != independent.right[v]);
  }
  assert(covered == best.size);
  for(auto [u,v] : graph.edges()) assert(cover.left[u] || cover.right[v]);
  const auto edge_cover = graph.minimum_edge_cover();
  std::vector<bool> a(graph.left_size()), b(graph.right_size());
  for(auto [u,v] : graph.edges()) a[u] = b[v] = true;
  const auto all = [](const auto& s) { return std::all_of(s.begin(),s.end(),[](bool x) { return x; }); };
  assert(bool(edge_cover) == (all(a) && all(b)));
  if(edge_cover) {
    assert(edge_cover->size() == a.size()+b.size()-best.size);
    std::fill(a.begin(),a.end(),false);
    std::fill(b.begin(),b.end(),false);
    for(int id : *edge_cover) {
      auto [u,v] = graph.get_edge(id);
      a[u] = b[v] = true;
    }
    assert(all(a) && all(b));
  }
  const auto dm = soryuusi::dulmage_mendelsohn_decomposition(graph);
  assert(dm.unique() == (best.count == 1));
  for(int id = 0; id < graph.edge_count(); ++id) {
    auto [u,v] = graph.get_edge(id);
    assert(dm.allowed[id] == bool(best.allowed>>id&1));
    assert(dm.essential[id] == bool(best.essential>>id&1));
    assert(dm.left_block[u] <= dm.right_block[v]);
    assert((dm.left_block[u] == dm.right_block[v]) == dm.allowed[id]);
  }
  std::vector<int> balance(dm.region.size());
  for(int x : dm.left_block) ++balance[x];
  for(int x : dm.right_block) --balance[x];
  using Region = soryuusi::DulmageMendelsohnDecomposition::Region;
  for(int i = 0; i < int(balance.size()); ++i) {
    assert(dm.region[i] == (balance[i] < 0 ? Region::right_deficient : balance[i] > 0 ? Region::left_deficient : Region::balanced));
  }
}

void random_graphs() {
  std::mt19937 rng(11415);
  for(int rep = 0; rep < 2300; ++rep) {
    const int l = rng()%7, r = rng()%7, m = l && r ? rng()%15 : 0;
    Graph graph(l,r);
    graph.reserve_edges(m);
    for(int i = 0; i < m; ++i) {
      assert(graph.add_edge(rng()%l,rng()%r) == i);
      if(i == m/2) check(graph);
    }
    check(graph);
  }
}

void boundaries() {
  constexpr int n = 180000;
  Graph graph(n,n);
  graph.reserve_edges(2*n-1);
  for(int i = 0; i < n-1; ++i) graph.add_edge(i,i+1);
  assert(graph.solve() == n-1);
  for(int i = 0; i < n; ++i) graph.add_edge(i,i);
  assert(graph.solve() == n);
  for(int i = 0; i < n; ++i) assert(graph.left_partner(i) == i && graph.right_partner(i) == i);
  const auto dm = soryuusi::dulmage_mendelsohn_decomposition(graph);
  assert(dm.unique() && dm.region.size() == n);
  Graph uneven(1,120000);
  for(int i = 100000; i < 120000; ++i) uneven.add_edge(0,i);
  assert(uneven.solve() == 1);
  assert(!uneven.minimum_edge_cover());
}

void preprocessing_cases() {
  constexpr int n = 150;
  std::mt19937 rng(20965);
  for(int rep = 0; rep < 30; ++rep) {
    Graph graph(n,n);
    soryuusi::MaxFlow<int> flow(2*n+2);
    for(int i = 0; i < n; ++i) {
      flow.add_edge(2*n,i,1);
      flow.add_edge(n+i,2*n+1,1);
    }
    for(int i = 0; i < (rep%2 ? 3 : 5)*n; ++i) {
      const int u = rng()%n, v = rng()%n;
      graph.add_edge(u,v);
      flow.add_edge(u,v+n,1);
    }
    assert(graph.solve() == flow.flow(2*n,2*n+1));
    const auto cover = graph.minimum_vertex_cover();
    for(auto [u,v] : graph.edges()) assert(cover.left[u] || cover.right[v]);
  }
  Graph cycle(n,n);
  for(int i = 0; i < n; ++i) {
    cycle.add_edge(i,(i+1)%n);
    cycle.add_edge(i,i);
  }
  assert(cycle.solve() == n && !soryuusi::dulmage_mendelsohn_decomposition(cycle).unique());
}

void dag_paths() {
  std::mt19937 rng(77522);
  for(int rep = 0; rep < 400; ++rep) {
    int n = rng()%7;
    Graph graph(n,n);
    for(int u = 0; u < n && graph.edge_count() < 15; ++u) {
      for(int v = u+1; v < n && graph.edge_count() < 15; ++v) if(rng()%2) graph.add_edge(u,v);
    }
    const auto paths = soryuusi::dag_path_cover(n,graph.edges());
    assert(paths.size() == std::size_t(n-enumerate(graph).size));
    std::vector<bool> used(n,false);
    const auto& edges = graph.edges();
    for(const auto& path : paths) for(int i = 0; i < int(path.size()); ++i) {
      const int v = path[i];
      assert(!used[v]);
      used[v] = true;
      if(i > 0) assert(std::find(edges.begin(),edges.end(),std::pair{path[i-1],v}) != edges.end());
    }
    assert(std::all_of(used.begin(),used.end(),[](bool x) { return x; }));
  }
}

} // namespace

int main() {
  random_graphs();
  boundaries();
  preprocessing_cases();
  dag_paths();
  std::cout << "OK\n";
}
