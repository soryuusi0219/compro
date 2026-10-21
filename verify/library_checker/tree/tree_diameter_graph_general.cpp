#include "library/utility/fastio.hpp"

#include <utility>
#include <vector>

#include "library/graph/graph_general.hpp"

int main() {
  int n;
  soryuusi::cin >> n;
  std::vector<std::vector<std::pair<int,long long>>> graph(n);
  for(int i = 1; i < n; ++i) {
    int u, v;
    long long w;
    soryuusi::cin >> u >> v >> w;
    graph[u].emplace_back(v,w);
    graph[v].emplace_back(u,w);
  }
  const auto [u,v,distance] = soryuusi::diameter(graph);
  const auto path = soryuusi::get_path(graph,u,v);
  soryuusi::cout << distance << ' ' << path.size() << '\n';
  for(int i = 0; i < static_cast<int>(path.size()); ++i) {
    soryuusi::cout << path[i] << " \n"[i + 1 == static_cast<int>(path.size())];
  }
}
