#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/data_structure/persistent_dsu.hpp"

int main() {

  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::Pdsu dsu;
  std::vector<soryuusi::np> versions(q + 1);
  versions[0] = dsu.init(n);

  for (int i = 0; i < q; ++i) {
    int type, k, u, v;
    soryuusi::cin >> type >> k >> u >> v;
    ++k;
    if (type == 0) {
      versions[i + 1] = dsu.merge(u, v, versions[k]).second;
    } else {
      soryuusi::cout << dsu.same(u, v, versions[k]) << '\n';
    }
  }
}
