#include "library/utility/fastio.hpp"

#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

#include "library/graph/static_graph.hpp"

int main() {
  int n, m, s, t;
  soryuusi::cin >> n >> m >> s >> t;
  soryuusi::static_graph<long long> graph(n,m);
  while(m--) {
    int u, v;
    long long w;
    soryuusi::cin >> u >> v >> w;
    graph.add_edge(u,v,w);
  }
  graph.build();
  const long long inf = std::numeric_limits<long long>::max();
  std::vector<long long> distance(n,inf);
  std::vector<int> parent(n,-1);
  std::priority_queue<std::pair<long long,int>,
                      std::vector<std::pair<long long,int>>,
                      std::greater<std::pair<long long,int>>> queue;
  distance[s] = 0;
  queue.emplace(0,s);
  while(!queue.empty()) {
    auto [d,v] = queue.top();
    queue.pop();
    if(distance[v] != d) continue;
    for(auto edge : graph[v]) {
      if(distance[edge.to] > d+edge.cost) {
        distance[edge.to] = d+edge.cost;
        parent[edge.to] = v;
        queue.emplace(distance[edge.to],edge.to);
      }
    }
  }
  if(distance[t] == inf) {
    soryuusi::cout << -1 << '\n';
    return 0;
  }
  std::vector<int> path;
  for(int v = t; v != s; v = parent[v]) path.push_back(v);
  path.push_back(s);
  soryuusi::cout << distance[t] << ' ' << path.size()-1 << '\n';
  for(int i = static_cast<int>(path.size())-1; i > 0; --i) {
    soryuusi::cout << path[i] << ' ' << path[i-1] << '\n';
  }
}
