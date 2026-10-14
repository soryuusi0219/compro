#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/dynamic_dsu.hpp"

int main() {

  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::dynamic_dsu d(n);
  while (q--) {
    int t,u,v; soryuusi::cin >> t >> u >> v;
    if (t == 0) d.merge(u,v);
    else soryuusi::cout << d.same(u,v) << '\n';
  }
}
