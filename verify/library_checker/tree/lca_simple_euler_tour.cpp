#include "library/utility/fastio.hpp"

#include "library/tree/simple_euler_tour.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::SimpleEularTour tree(n);
  for(int v = 1; v < n; ++v) {
    int parent;
    soryuusi::cin >> parent;
    tree.add_edge(parent,v);
  }
  tree.build();
  while(q--) {
    int u, v;
    soryuusi::cin >> u >> v;
    soryuusi::cout << tree.lca(u,v) << '\n';
  }
}
