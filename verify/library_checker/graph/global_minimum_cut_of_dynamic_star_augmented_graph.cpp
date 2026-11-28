#include <vector>

#include "library/graph/dynamic_star_minimum_cut.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int n,m,q;
  soryuusi::cin >> n >> m >> q;
  std::vector<long long> weights(n);
  for(auto& w : weights) soryuusi::cin >> w;
  std::vector<soryuusi::UndirectedCutEdge<long long>> edges(m);
  for(auto& [u,v,w] : edges) soryuusi::cin >> u >> v >> w;
  soryuusi::DynamicStarMinimumCut graph(weights,edges);
  while(q--) {
    int v;
    long long w;
    soryuusi::cin >> v >> w;
    soryuusi::cout << graph.set(v,w) << '\n';
  }
}
