#include "library/graph/max_flow.hpp"
#include "library/graph/minimum_cut_lattice.hpp"
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
  graph.flow(0,n-1);
  const auto lattice = soryuusi::minimum_cut_lattice(graph,0,n-1);
  const auto closest = lattice->minimum_source_side(), farthest = lattice->maximum_source_side();
  int near_cost = 0, far_cost = 0;
  for(const auto& [u,v,cap,flow] : graph.edges()) {
    if(closest[u] && !closest[v]) near_cost += cap;
    if(farthest[u] && !farthest[v]) far_cost += cap;
  }
  soryuusi::cout << (near_cost == far_cost ? near_cost : -1) << '\n';
}
