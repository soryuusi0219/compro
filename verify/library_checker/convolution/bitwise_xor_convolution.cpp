#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/math/modint.hpp"

#include "library/math/convolution_bit_xor.hpp"

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
  const auto result = soryuusi::convolution_bitxor(a, b);
  for (int i = 0; i < (1 << n); ++i) {
    soryuusi::cout << result[i].val() << " \n"[i + 1 == (1 << n)];
  }
}
