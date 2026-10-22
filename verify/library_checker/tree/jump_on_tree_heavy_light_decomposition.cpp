#include "library/utility/fastio.hpp"

#include "library/tree/heavy_light_decomposition.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::HLdecomposition tree(n);
  for(int i = 1; i < n; ++i) {
    int u, v;
    soryuusi::cin >> u >> v;
    tree.add_edge(u,v);
  }
  tree.build();
  while(q--) {
    int s, t, i;
    soryuusi::cin >> s >> t >> i;
    soryuusi::cout << tree.jump(s,t,i) << '\n';
  }
}
