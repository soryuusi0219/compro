#include "library/graph/bipartite_matching.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int l,r,m;
  soryuusi::cin >> l >> r >> m;
  soryuusi::BipartiteMatching graph(l,r);
  graph.reserve_edges(m);
  for(int i = 0; i < m; ++i) {
    int u,v;
    soryuusi::cin >> u >> v;
    graph.add_edge(u,v);
  }
  soryuusi::cout << graph.solve() << '\n';
  for(int u = 0; u < l; ++u) {
    const int v = graph.left_partner(u);
    if(v >= 0) soryuusi::cout << u << ' ' << v << '\n';
  }
}
