#include "library/utility/fastio.hpp"

#include "library/tree/heavy_light_decomposition.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::HLdecomposition tree(n);
  for(int vertex = 1; vertex < n; ++vertex) {
    int parent;
    soryuusi::cin >> parent;
    tree.add_edge(parent,vertex);
  }
  tree.build(0);
  while(q--) {
    int u, v;
    soryuusi::cin >> u >> v;
    soryuusi::cout << tree.lca(u,v) << '\n';
  }
}
