#include <algorithm>
#include <string>
#include "library/graph/project_selection.hpp"
#include "library/utility/fastio.hpp"

int main() {
  int n,m;
  std::string direction;
  soryuusi::cin >> n >> m >> direction;
  soryuusi::ProjectSelection<> model(n);
  model.reserve_edges(m);
  for(int i = 0; i < n; ++i) {
    long long cost;
    soryuusi::cin >> cost;
    model.add_cost(i,cost,direction[i] == 'L');
  }
  for(int i = 0; i < m; ++i) {
    int u,v;
    long long cost;
    soryuusi::cin >> u >> v >> cost;
    if(u > v) std::swap(u,v);
    model.add_implication_penalty(u-1,v-1,cost);
  }
  soryuusi::cout << model.solve()->cost << '\n';
}
