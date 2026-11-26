#include "library/graph/min_cost_flow.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int n,m,f;
  soryuusi::cin >> n >> m >> f;
  soryuusi::MinCostFlow<int,int,long long> graph(n);
  graph.reserve_edges(m);
  for(int i = 0; i < m; ++i) {
    int u,v,cap,cost;
    soryuusi::cin >> u >> v >> cap >> cost;
    graph.add_edge(u,v,cap,cost);
  }
  const auto [sent,cost] = graph.flow(0,n-1,f);
  soryuusi::cout << (sent == f ? cost : -1) << '\n';
}
