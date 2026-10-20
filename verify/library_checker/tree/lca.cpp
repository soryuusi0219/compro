#include "library/utility/fastio.hpp"

#include "library/tree/lca_doubling.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::lca_doubling tree(n);
  for(int vertex = 1; vertex < n; ++vertex) {
    int parent;
    soryuusi::cin >> parent;
    tree.add_edge(parent,vertex);
  }
  tree.build();
  while(q--) {
    int u, v;
    soryuusi::cin >> u >> v;
    soryuusi::cout << tree.lca(u,v) << '\n';
  }
}
