#include "library/utility/fastio.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include "library/graph/biconnected_graph.hpp"

namespace {

bool validate_block_cut_tree(
    const std::vector<std::vector<int>>& tree,
    const std::vector<int>& index,
    const std::vector<std::vector<int>>& components) {
  const int n = static_cast<int>(index.size());
  const int node_count = static_cast<int>(tree.size());
  std::vector<int> membership_count(n,0);
  for(const auto& component : components) {
    for(int vertex : component) {
      if(vertex < 0 || vertex >= n) return false;
      ++membership_count[vertex];
    }
  }
  for(int count : membership_count) if(count == 0) return false;

  std::vector<std::vector<int>> mapped_vertices(node_count);
  for(int vertex = 0; vertex < n; ++vertex) {
    if(index[vertex] < 0 || index[vertex] >= node_count) return false;
    mapped_vertices[index[vertex]].push_back(vertex);
  }

  std::map<int,int> articulation_vertex;
  for(int vertex = 0; vertex < n; ++vertex) {
    if(membership_count[vertex] <= 1) continue;
    if(!articulation_vertex.emplace(index[vertex],vertex).second) return false;
  }

  std::set<std::pair<int,int>> directed_edges;
  for(int u = 0; u < node_count; ++u) {
    std::set<int> adjacent;
    for(int v : tree[u]) {
      if(v < 0 || v >= node_count || v == u || !adjacent.insert(v).second) {
        return false;
      }
      directed_edges.emplace(u,v);
    }
  }
  for(const auto& [u,v] : directed_edges) {
    if(!directed_edges.count({v,u})) return false;
  }

  std::multiset<std::vector<int>> actual_components;
  for(int node = 0; node < node_count; ++node) {
    const auto articulation = articulation_vertex.find(node);
    if(articulation != articulation_vertex.end()) {
      if(mapped_vertices[node] != std::vector<int>{articulation->second}) return false;
      for(int next : tree[node]) {
        if(articulation_vertex.count(next)) return false;
      }
      continue;
    }

    std::vector<int> component = mapped_vertices[node];
    for(int vertex : component) {
      if(membership_count[vertex] != 1) return false;
    }
    for(int next : tree[node]) {
      const auto it = articulation_vertex.find(next);
      if(it == articulation_vertex.end()) return false;
      component.push_back(it->second);
    }
    std::sort(component.begin(),component.end());
    if(std::adjacent_find(component.begin(),component.end()) != component.end()) {
      return false;
    }
    actual_components.insert(std::move(component));
  }

  std::multiset<std::vector<int>> expected_components;
  for(auto component : components) {
    std::sort(component.begin(),component.end());
    expected_components.insert(std::move(component));
  }
  return actual_components == expected_components;
}

}  // namespace

int main() {
  int n, m;
  soryuusi::cin >> n >> m;
  soryuusi::biconnected_graph graph(n,m);
  while(m--) {
    int u, v;
    soryuusi::cin >> u >> v;
    graph.add_edge(u,v);
  }

  const auto components = graph.two_vertex_connected_components();
  const auto [tree,index] = graph.block_cut_tree();
  if(!validate_block_cut_tree(tree,index,components)) return 1;

  soryuusi::cout << components.size() << '\n';
  for(const auto& component : components) {
    soryuusi::cout << component.size();
    for(int vertex : component) soryuusi::cout << ' ' << vertex;
    soryuusi::cout << '\n';
  }
}
