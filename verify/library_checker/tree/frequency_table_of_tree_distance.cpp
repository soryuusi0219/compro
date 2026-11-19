#include "library/tree/tree_distance_frequency.hpp"
#include "library/utility/fastio.hpp"

int main(){
  int n;
  soryuusi::cin >> n;
  soryuusi::TreeDistanceFrequency tree(n);
  for(int i = 1; i < n; ++i){
    int u,v;
    soryuusi::cin >> u >> v;
    tree.add_edge(u,v);
  }
  auto counts = tree.solve();
  for(int d = 1; d < n; ++d) soryuusi::cout << counts[d] << " \n"[d+1 == n];
}
