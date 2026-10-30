#include "library/utility/fastio.hpp"

#include <limits>

#include "library/graph/dijkstra_1.hpp"

int main() {
  int n;
  soryuusi::cin >> n;
  soryuusi::Dijkstra1 graph(n);
  for(int i = 0; i < n; ++i) {
    int u, k;
    soryuusi::cin >> u >> k;
    --u;
    while(k--) {
      int v;
      soryuusi::cin >> v;
      graph.add_edge(u,v-1);
    }
  }
  const auto distance = graph.distance(0);
  for(int i = 0; i < n; ++i) {
    const int d = distance[i] == std::numeric_limits<int>::max() ? -1 : distance[i];
    soryuusi::cout << i+1 << ' ' << d << '\n';
  }
}
