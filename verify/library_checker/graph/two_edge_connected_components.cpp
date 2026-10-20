#include "library/utility/fastio.hpp"

#include "library/graph/biconnected_graph.hpp"

int main() {
  int n, m;
  soryuusi::cin >> n >> m;
  soryuusi::biconnected_graph graph(n);
  while(m--) {
    int u, v;
    soryuusi::cin >> u >> v;
    graph.add_edge(u,v);
  }
  const auto [tree,index,components] = graph.two_edge_connected_components();
  (void)tree;
  (void)index;
  soryuusi::cout << components.size() << '\n';
  for(const auto& component : components) {
    soryuusi::cout << component.size();
    for(long long vertex : component) soryuusi::cout << ' ' << vertex;
    soryuusi::cout << '\n';
  }
}
