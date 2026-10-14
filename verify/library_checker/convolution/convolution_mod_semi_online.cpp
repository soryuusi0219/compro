#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/math/modint.hpp"
#include "library/math/convolution_semi_online.hpp"

int main() {
  using Mint = soryuusi::static_modint<998244353>;
  using NTT = soryuusi::NTTEngine<Mint>;


  int n, m;
  soryuusi::cin >> n >> m;
  std::vector<Mint> a(n+m-1,0), b(m);
  for (int i = 0; i < n; ++i) {
    soryuusi::cin >> a[i];
  }
  for (int i = 0; i < m; ++i) {
    soryuusi::cin >> b[i];
  }

  soryuusi::semi_online_convolution<Mint> oc(std::move(a));
  

  for (int i = 0; i < n + m - 1; ++i) {
    soryuusi::cout << oc.push(
      i < m ? b[i] : 0
    ) << " \n"[i + 1 == n + m - 1];
  }
}
