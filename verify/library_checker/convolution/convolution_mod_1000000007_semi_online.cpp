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
  for (int i = 0; i < n; ++i) {
    soryuusi::cin >> a[i];
  }
  for (int i = 0; i < m; ++i) {
    soryuusi::cin >> b[i];
  }

  soryuusi::online_convolution<Mint> oc(n+m-1);
  

  for (int i = 0; i < n + m - 1; ++i) {
    soryuusi::cout << oc.push(
      i < n ? a[i] : 0,
      i < m ? b[i] : 0
    ) << " \n"[i + 1 == n + m - 1];
  }
}
