#include "library/utility/fastio.hpp"

#include "library/data_structure/weighted_dsu.hpp"
#include "library/math/modint.hpp"

using mint = soryuusi::modint998244353;

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::Weighted_dsu_sum<mint> dsu(n);
  while (q--) {
    int t, u, v;
    soryuusi::cin >> t >> u >> v;
    if (t == 0) {
      int x;
      soryuusi::cin >> x;
      soryuusi::cout << (dsu.merge(v, u, mint(x)) != -1) << '\n';
    } else if (!dsu.same(u, v)) {
      soryuusi::cout << -1 << '\n';
    } else {
      soryuusi::cout << dsu.weight(v, u).val() << '\n';
    }
  }
}
