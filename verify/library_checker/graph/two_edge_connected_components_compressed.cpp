#include "library/utility/fastio.hpp"

#include <cstdio>
#include <set>
#include <utility>
#include <vector>

#include "library/graph/biconnected_graph.hpp"

namespace {

bool validate_compressed_graph(
    const std::vector<std::pair<int,int>>& edges,
    const std::vector<std::vector<int>>& compressed,
    const std::vector<int>& index,
    const std::vector<std::vector<int>>& components) {
  const int component_count = static_cast<int>(components.size());
  if(static_cast<int>(compressed.size()) != component_count) {
    std::fprintf(stderr,"component count mismatch\n");
    return false;
  }

  std::vector<int> seen(index.size(),0);
  for(int component = 0; component < component_count; ++component) {
    for(int vertex : components[component]) {
      if(vertex < 0 || vertex >= static_cast<int>(index.size())) {
        std::fprintf(stderr,"invalid component vertex\n");
        return false;
      }
      if(++seen[vertex] != 1 || index[vertex] != component) {
        std::fprintf(stderr,"component index mismatch\n");
        return false;
      }
    }
  }
  for(int vertex = 0; vertex < static_cast<int>(index.size()); ++vertex) {
    if(seen[vertex] != 1 || index[vertex] < 0 || index[vertex] >= component_count) {
      std::fprintf(stderr,"invalid vertex index\n");
      return false;
    }
  }

  std::set<std::pair<int,int>> directed_edges;
  std::set<std::pair<int,int>> actual_edges;
  for(int u = 0; u < component_count; ++u) {
    std::set<int> adjacent;
    for(int v : compressed[u]) {
      if(v < 0 || v >= component_count || v == u || !adjacent.insert(v).second) {
        std::fprintf(stderr,"invalid compressed edge\n");
        return false;
      }
      directed_edges.emplace(u,v);
      if(u < v) actual_edges.emplace(u,v);
    }
  }
  for(const auto& [u,v] : directed_edges) {
    if(!directed_edges.count({v,u})) {
      std::fprintf(stderr,"asymmetric compressed edge\n");
      return false;
    }
  }

  std::set<std::pair<int,int>> expected_edges;
  for(const auto& [u,v] : edges) {
    int x = index[u];
    int y = index[v];
    if(x == y) continue;
    if(x > y) std::swap(x,y);
    expected_edges.emplace(x,y);
  }
  if(actual_edges != expected_edges) {
    std::fprintf(stderr,"compressed edge set mismatch: %zu != %zu\n",
                 actual_edges.size(),expected_edges.size());
    for(const auto& edge : expected_edges) {
      if(!actual_edges.count(edge)) {
        std::fprintf(stderr,"missing edge %d %d\n",edge.first,edge.second);
        break;
      }
    }
    for(const auto& edge : actual_edges) {
      if(!expected_edges.count(edge)) {
        std::fprintf(stderr,"extra edge %d %d\n",edge.first,edge.second);
        break;
      }
    }
    return false;
  }
  return true;
}

}  // namespace

int main() {
  int n, m;
  soryuusi::cin >> n >> m;
  soryuusi::biconnected_graph graph(n,m);
  std::vector<std::pair<int,int>> edges(m);
  for(auto& [u,v] : edges) {
    soryuusi::cin >> u >> v;
    graph.add_edge(u,v);
  }

  const auto [compressed,index,components] = graph.two_edge_connected_compressed();
  if(!validate_compressed_graph(edges,compressed,index,components)) return 1;

  soryuusi::cout << components.size() << '\n';
  for(const auto& component : components) {
    soryuusi::cout << component.size();
    for(int vertex : component) soryuusi::cout << ' ' << vertex;
    soryuusi::cout << '\n';
  }
}
