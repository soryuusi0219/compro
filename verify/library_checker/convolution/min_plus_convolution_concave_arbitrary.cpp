#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/math/convolution_min_plus_concave_arbitrary.hpp"

int main() {

  int n, m;
  soryuusi::cin >> n >> m;
  std::vector<long long> a(n), b(m);
  for(auto& x : a) soryuusi::cin >> x;
  for(auto& x : b) soryuusi::cin >> x;
  const auto result = soryuusi::min_plus_convolution_conc_arb(a, b);
  for (int i = 0; i < n+m-1; ++i) {
    soryuusi::cout << result[i] << " \n"[i + 1 == n+m-1];
  }
}
