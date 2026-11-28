#include "library/graph/general_matching.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int n,m;
  soryuusi::cin >> n >> m;
  soryuusi::GeneralMatching graph(n);
  graph.reserve_edges(m);
  for(int i = 0; i < m; ++i) {
    int u,v;
    soryuusi::cin >> u >> v;
    graph.add_edge(u,v);
  }
  soryuusi::cout << graph.solve() << '\n';
  for(int id : graph.matching_edges()) {
    const auto [u,v] = graph.get_edge(id);
    soryuusi::cout << u << ' ' << v << '\n';
  }
}
