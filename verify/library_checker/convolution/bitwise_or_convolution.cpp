#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/math/modint.hpp"

#include "library/math/convolution_bit_or.hpp"

using mint = soryuusi::modint998244353;

int main() {

  int n;
  soryuusi::cin >> n;
  std::vector<mint> a(1 << n), b(1 << n);
  for (mint& value : a) {
    int input;
    soryuusi::cin >> input;
    value = input;
  }
  for (mint& value : b) {
    int input;
    soryuusi::cin >> input;
    value = input;
  }
  reverse(a.begin(), a.end());
  reverse(b.begin(), b.end());
  const auto result = soryuusi::convolution_bitor(a, b);
  for (int i = (1 << n) - 1; i >= 0; --i) {
    soryuusi::cout << result[i].val() << " \n"[i == 0];
  }
}
