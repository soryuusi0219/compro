#include "library/utility/fastio.hpp"

#include <utility>

#include "library/graph/min_cost_flow_modified.hpp"

using CostFunction = std::pair<long long,long long>;

std::pair<long long,long long> forward_cost(CostFunction f, long long) {
  return f;
}
std::pair<long long,long long> backward_cost(CostFunction f, long long) {
  return {f.first,0};
}
long long max_cost() { return 1LL << 60; }
long long zero_cost() { return 0; }

int main() {
  using Graph = soryuusi::mcf_graph_modified<
      long long,long long,CostFunction,forward_cost,backward_cost,max_cost,zero_cost>;
  int n, m;
  long long required;
  soryuusi::cin >> n >> m >> required;
  Graph graph(n);
  while(m--) {
    int u, v;
    long long capacity, cost;
    soryuusi::cin >> u >> v >> capacity >> cost;
    graph.add_edge(u,v,capacity,{cost,capacity});
  }
  const auto [flow,cost] = graph.flow(0,n-1,required);
  soryuusi::cout << (flow == required ? cost : -1) << '\n';
}
