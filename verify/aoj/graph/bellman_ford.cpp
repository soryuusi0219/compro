#include "library/utility/fastio.hpp"

#include <limits>

#include "library/graph/bellman_ford.hpp"

int main() {
  int n, m, source;
  soryuusi::cin >> n >> m >> source;
  soryuusi::BellmanFordll graph(n);
  while(m--) {
    int u, v;
    long long cost;
    soryuusi::cin >> u >> v >> cost;
    graph.add_edge(u,v,cost);
  }
  const auto [ok,distance] = graph.distance(source);
  if(!ok) {
    soryuusi::cout << "NEGATIVE CYCLE\n";
    return 0;
  }
  for(long long x : distance) {
    if(x == (1LL<<60)) soryuusi::cout << "INF\n";
    else soryuusi::cout << x << '\n';
  }
}
