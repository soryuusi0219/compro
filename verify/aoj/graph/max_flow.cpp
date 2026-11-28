#include "library/graph/max_flow.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int n,m;
  soryuusi::cin >> n >> m;
  soryuusi::MaxFlow<int> graph(n);
  graph.reserve_edges(m);
  for(int i = 0; i < m; ++i) {
    int u,v,c;
    soryuusi::cin >> u >> v >> c;
    graph.add_edge(u,v,c);
  }
  soryuusi::cout << graph.flow(0,n-1) << '\n';
}
