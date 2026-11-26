#include <vector>
#include "library/graph/min_cost_b_flow.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int n,m;
  soryuusi::cin >> n >> m;
  std::vector<long long> supply(n);
  for(auto& b : supply) soryuusi::cin >> b;
  soryuusi::MinCostBFlow<> graph(n);
  graph.reserve_edges(m);
  for(int i = 0; i < m; ++i) {
    int u,v;
    long long lo,hi,cost;
    soryuusi::cin >> u >> v >> lo >> hi >> cost;
    graph.add_edge(u,v,lo,hi,cost);
  }
  const auto answer = graph.solve(supply);
  if(!answer) {
    soryuusi::cout << "infeasible\n";
    return 0;
  }
  soryuusi::cout << answer->cost << '\n';
  for(auto p : answer->potential) soryuusi::cout << p << '\n';
  for(auto f : answer->flow) soryuusi::cout << f << '\n';
}
