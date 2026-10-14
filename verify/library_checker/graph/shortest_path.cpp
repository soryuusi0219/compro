#include <vector>

#include "library/graph/dijkstra.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int n, m, source, target;
  soryuusi::cin >> n >> m >> source >> target;

  soryuusi::Dijkstrall graph(n, m);
  for (int i = 0; i < m; ++i) {
    int from, to;
    long long cost;
    soryuusi::cin >> from >> to >> cost;
    graph.add_edge(from, to, cost);
  }

  const auto distance = graph.distance(source);
  if (distance[target] == soryuusi::infty()) {
    soryuusi::cout << -1 << '\n';
    return 0;
  }

  const std::vector<int> path = graph.path(target);
  soryuusi::cout << distance[target] << ' ' << path.size() - 1 << '\n';
  for (int i = 1; i < static_cast<int>(path.size()); ++i) {
    soryuusi::cout << path[i - 1] << ' ' << path[i] << '\n';
  }
}
