#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/math/modint.hpp"
#include "library/math/convolution_online.hpp"

int main() {
  using Mint = soryuusi::static_modint<1000000007>;

  int n, m;
  soryuusi::cin >> n >> m;
  std::vector<Mint> a(n), b(m);
  for (Mint& x : a) {
    soryuusi::cin >> x;
  }
  for (Mint& x : b) {
    soryuusi::cin >> x;
  }

  const auto result = soryuusi::convolution_mod(std::move(a), std::move(b));
  for (int i = 0; i < n + m - 1; ++i) {
    soryuusi::cout << result[i].val() << " \n"[i + 1 == n + m - 1];
  }
}
