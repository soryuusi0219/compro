#include "library/utility/fastio.hpp"

#include <map>
#include <utility>

#include "library/graph/prim.hpp"

int main() {
  int n, m;
  soryuusi::cin >> n >> m;
  soryuusi::Prim<long long> graph(n);
  std::map<std::pair<int,int>,std::pair<long long,int>> best_edge;
  for(int i = 0; i < m; ++i) {
    int u, v;
    long long cost;
    soryuusi::cin >> u >> v >> cost;
    graph.add_edge(u,v,cost);
    if(u > v) std::swap(u,v);
    const auto key = std::pair(u,v);
    if(!best_edge.count(key) || cost < best_edge[key].first) {
      best_edge[key] = {cost,i};
    }
  }
  const auto [cost,edges] = graph.solve();
  soryuusi::cout << cost << '\n';
  for(const auto& [first,second] : edges) {
    int u = static_cast<int>(first);
    int v = static_cast<int>(second);
    if(u > v) std::swap(u,v);
    soryuusi::cout << best_edge[{u,v}].second << ' ';
  }
  soryuusi::cout << '\n';
}
