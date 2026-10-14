#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/math/modint.hpp"

#include "library/math/convolution_gcd_lcm.hpp"

using mint = soryuusi::modint998244353;

int main() {

  int n;
  soryuusi::cin >> n;
  std::vector<mint> a(n + 1), b(n + 1);
  for (int i = 1; i <= n; ++i) {
    int input;
    soryuusi::cin >> input;
    a[i] = input;
  }
  for (int i = 1; i <= n; ++i) {
    int input;
    soryuusi::cin >> input;
    b[i] = input;
  }
  const auto result = soryuusi::convolution_gcd(a, b);
  for (int i = 1; i <= n; ++i) {
    soryuusi::cout << result[i].val() << " \n"[i == n];
  }
}
