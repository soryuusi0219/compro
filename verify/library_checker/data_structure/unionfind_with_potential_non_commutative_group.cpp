#include "library/utility/fastio.hpp"
#include <array>

#include "library/data_structure/weighted_dsu.hpp"
#include "library/math/modint.hpp"

using mint = soryuusi::modint998244353;
using Matrix = std::array<mint, 4>;

constexpr auto multiply = [](const Matrix& a, const Matrix& b)->Matrix {
  return {a[0]*b[0] + a[1]*b[2], a[0]*b[1] + a[1]*b[3],
          a[2]*b[0] + a[3]*b[2], a[2]*b[1] + a[3]*b[3]};
};
constexpr auto identity = []()->Matrix { return {1, 0, 0, 1}; };
// Every matrix in the problem has determinant one.
constexpr auto inverse = [](const Matrix& a)->Matrix {
  return {a[3], -a[1], -a[2], a[0]};
};

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::Weighted_dsu<Matrix, multiply, identity, inverse> dsu(n);
  // Store a_i^{-1} as the potential, so weight(u, v) = a_v^{-1} * a_u.
  while (q--) {
    int t, u, v;
    soryuusi::cin >> t >> u >> v;
    if (t == 0) {
      Matrix x;
      for (auto& value : x) {
        int entry;
        soryuusi::cin >> entry;
        value = entry;
      }
      soryuusi::cout << (dsu.merge(u, v, x) != -1) << '\n';
    } else if (!dsu.same(u, v)) {
      soryuusi::cout << -1 << '\n';
    } else {
      const Matrix x = dsu.weight(u, v);
      soryuusi::cout << x[0].val() << ' ' << x[1].val() << ' '
                     << x[2].val() << ' ' << x[3].val() << '\n';
    }
  }
}
