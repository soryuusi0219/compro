#include <vector>

#include "library/graph/directed_mst.hpp"
#include "library/utility/fastio.hpp"

int main(){
  int n,m,root;
  soryuusi::cin >> n >> m >> root;
  soryuusi::DirectedMST<long long> graph(n,m);
  std::vector<int> from(m);
  for(int i = 0; i < m; ++i){
    int to;
    long long cost;
    soryuusi::cin >> from[i] >> to >> cost;
    graph.add_edge(from[i],to,cost);
  }
  auto [cost,incoming] = graph.solve(root);
  soryuusi::cout << cost << '\n';
  for(int v = 0; v < n; ++v) soryuusi::cout << (v == root ? root : from[incoming[v]]) << " \n"[v+1 == n];
}
