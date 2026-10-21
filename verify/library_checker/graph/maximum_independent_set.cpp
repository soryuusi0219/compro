#include "library/utility/fastio.hpp"

#include <vector>

#include "library/graph/maximal_independent_set.hpp"

int main() {
  int n, m;
  soryuusi::cin >> n >> m;
  std::vector<std::vector<long long>> graph(n);
  while(m--) {
    int u, v;
    soryuusi::cin >> u >> v;
    graph[u].push_back(v);
    graph[v].push_back(u);
  }
  const auto answer = soryuusi::maximal_independent_set(graph);
  soryuusi::cout << answer.size() << '\n';
  for(int i = 0; i < static_cast<int>(answer.size()); ++i) {
    soryuusi::cout << answer[i] << " \n"[i + 1 == static_cast<int>(answer.size())];
  }
}
