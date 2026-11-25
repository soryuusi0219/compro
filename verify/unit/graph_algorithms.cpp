#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <random>
#include <set>
#include <utility>
#include <vector>

#include "library/graph/bellman_ford.hpp"
#include "library/graph/biconnected_graph.hpp"
#include "library/graph/complement_graph_connected_components.hpp"
#include "library/graph/dijkstra.hpp"
#include "library/graph/dijkstra_01.hpp"
#include "library/graph/enumerate_triangles.hpp"
#include "library/data_structure/dsu.hpp"
#include "library/graph/prim.hpp"
#include "library/graph/scc.hpp"

namespace {

void test_connectivity(){
  std::mt19937 gen(5051);
  for(int n = 1; n <= 10; ++n) for(int trial = 0; trial < 20; ++trial){
    std::vector<std::pair<int,int>> edges;
    soryuusi::biconnected_graph graph(n);
    soryuusi::SCC directed(n);
    soryuusi::ComplementConnectedComponents complement(n);
    std::vector<std::vector<bool>> adjacent(n,std::vector<bool>(n)), reach(n,std::vector<bool>(n));
    for(int u = 0; u < n; ++u){
      reach[u][u] = true;
      for(int v = u; v < n; ++v) if(gen()%4 == 0){
        if(u != v){
          adjacent[u][v] = adjacent[v][u] = true;
          complement.add_edge(u,v);
        }
        directed.add_edge(u,v);
        reach[u][v] = true;
        if(gen()%3 == 0) directed.add_edge(v,u), reach[v][u] = true;
        for(int k = 1+gen()%2; k; --k) graph.add_edge(u,v), edges.emplace_back(u,v);
      }
    }
    auto components_after = [&](int edge_skip, int vertex_skip){
      soryuusi::DSU dsu(n);
      for(int i = 0; i < (int)edges.size(); ++i){
        auto [u,v] = edges[i];
        if(i != edge_skip && u != vertex_skip && v != vertex_skip) dsu.merge(u,v);
      }
      return dsu;
    };
    soryuusi::DSU bridge_free(n), inverse(n);
    for(int i = 0; i < (int)edges.size(); ++i){
      auto [u,v] = edges[i];
      const bool bridge = !components_after(i,-1).same(u,v);
      assert(graph.is_bridge(u,v) == bridge);
      assert(graph.is_bridge(v,u) == bridge);
      if(!bridge) bridge_free.merge(u,v);
    }
    for(int u = 0; u < n; ++u) for(int v = u+1; v < n; ++v) if(!adjacent[u][v]) inverse.merge(u,v);
    const auto [forest,group,components] = graph.two_edge_connected_compressed();
    const auto blocks = graph.two_vertex_connected_components();
    const auto [count,dag,ids] = directed.scc_compressed();
    std::vector<int> inverse_ids(n,-1);
    int component_count = 0;
    for(const auto& vertices : complement.solve()){
      for(int v : vertices) inverse_ids[v] = component_count;
      ++component_count;
    }
    for(int k = 0; k < n; ++k) for(int u = 0; u < n; ++u) for(int v = 0; v < n; ++v)
      reach[u][v] = reach[u][v] || (reach[u][k] && reach[k][v]);
    auto connected = components_after(-1,-1);
    for(int u = 0; u < n; ++u){
      assert(inverse_ids[u] >= 0 && 0 <= ids[u] && ids[u] < count);
      auto one_component = graph.two_edge_connected_component(u);
      std::sort(one_component.begin(),one_component.end());
      auto full_component = components[group[u]];
      std::sort(full_component.begin(),full_component.end());
      assert(one_component == full_component);
      for(int v = 0; v < n; ++v){
        assert((group[u] == group[v]) == bridge_free.same(u,v));
        assert((inverse_ids[u] == inverse_ids[v]) == inverse.same(u,v));
        assert((ids[u] == ids[v]) == (reach[u][v] && reach[v][u]));
        if(reach[u][v]) assert(ids[u] <= ids[v]);
        bool same_block = connected.same(u,v);
        for(int w = 0; w < n; ++w) if(w != u && w != v && same_block) same_block = components_after(-1,w).same(u,v);
        bool found = false;
        for(const auto& block : blocks)
          found |= std::find(block.begin(),block.end(),u) != block.end() && std::find(block.begin(),block.end(),v) != block.end();
        assert(found == same_block);
      }
    }
    for(int g = 0; g < count; ++g) for(int to : dag[g]) assert(g < to);
    for(int i = 0; i < (int)forest.size(); ++i) for(int j : forest[i]) assert(i != j);
  }
}

void test_shortest_and_spanning(){
  const auto plus = [](long long a, long long b){return a+b;};
  const auto zero = []{return 0LL;};
  const auto inf = []{return 1LL<<60;};
  std::mt19937 gen(3560);
  for(int n = 1; n <= 45; ++n){
    soryuusi::Dijkstra01 binary(n);
    soryuusi::Dijkstrall radix(n), general_weight(n);
    soryuusi::Dijkstra<long long,plus,zero,inf> generic(n);
    for(int i = 0; i < 4*n; ++i){
      int u = gen()%n, v = gen()%n, w = gen()%2;
      binary.add_edge(u,v,w);
      radix.add_edge(u,v,w);
      const long long weight = (gen()%2) ? gen()%100 : gen()*(1LL<<21);
      generic.add_edge(u,v,weight);
      general_weight.add_edge(u,v,weight);
    }
    for(int source : {0,n/2,n-1}){
      const auto a = binary.distance(source);
      const auto b = radix.distance(source);
      for(int v = 0; v < n; ++v) assert(a[v] == (b[v] == radix.INF ? std::numeric_limits<int>::max() : b[v]));
      const auto c = generic.distance(source);
      const auto d = general_weight.distance(source);
      for(int v = 0; v < n; ++v) assert(c[v] == (d[v] == general_weight.INF ? inf() : d[v]));
    }
  }
  soryuusi::Dijkstra_sum<unsigned long long> overflow(3);
  overflow.add_edge(0,1,5);
  overflow.add_edge(1,2,std::numeric_limits<unsigned long long>::max()-1);
  assert(overflow.distance(0)[2] == overflow.INF);
  soryuusi::Prim<long long> prim(4);
  prim.add_edge(0,1,-3);
  prim.add_edge(1,2,5);
  prim.add_edge(1,3,1);
  prim.add_edge(2,3,2);
  for(int root = 0; root < 4; ++root){
    const auto [weight,edges] = prim.solve(root);
    assert(weight == 0 && edges.size() == 3 && edges.front().first == root);
  }

  soryuusi::BellmanFordll bellman(4);
  bellman.add_edge(2,3,-2);
  bellman.add_edge(3,2,1);
  assert(bellman.distance(0).first);
  bellman.add_edge(1,2,10);
  bellman.add_edge(0,1,4);
  assert(!bellman.distance(0).first);
}

void test_deep_and_triangles(){
  const int n = 250000;
  soryuusi::biconnected_graph path(n,n-1);
  soryuusi::SCC directed_path(n,n-1);
  for(int v = 1; v < n; ++v) path.add_edge(v-1,v), directed_path.add_edge(v-1,v);
  assert(path.two_edge_connected_components().size() == n);
  assert(path.two_vertex_connected_components().size() == n-1);
  const auto [count,ids] = directed_path.scc_ids();
  assert(count == n && ids.front() == 0 && ids.back() == n-1);
  for(int extra : {0,60}){
    soryuusi::EnumerateTriangles graph(4+extra);
    for(int u = 0; u < 4; ++u) for(int v = u+1; v < 4; ++v) graph.add_edge(u,v);
    for(int run = 0; run < 2; ++run){
      std::set<int> masks;
      graph.solve([&](int u,int v,int w){assert(masks.insert((1<<u)|(1<<v)|(1<<w)).second);});
      assert(masks == std::set<int>({7,11,13,14}));
    }
  }
}

}  // namespace

int main(){
  test_connectivity();
  test_shortest_and_spanning();
  test_deep_and_triangles();
}
