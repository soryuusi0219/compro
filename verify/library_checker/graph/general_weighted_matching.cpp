#include "library/graph/general_weighted_matching.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int n,m;
  soryuusi::cin >> n >> m;
  soryuusi::GeneralWeightedMatching<int> graph(n);
  graph.reserve_edges(m);
  for(int i = 0; i < m; ++i) {
    int u,v,w;
    soryuusi::cin >> u >> v >> w;
    graph.add_edge(u,v,w);
  }
  const auto result = *graph.solve();
  soryuusi::cout << result.cardinality << ' ' << result.weight << '\n';
  for(int id : graph.matching_edges()) {
    const auto edge = graph.get_edge(id);
    soryuusi::cout << edge.u << ' ' << edge.v << '\n';
  }
}
