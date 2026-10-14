#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/range_parallel_unionfind.hpp"

#include "library/math/modint.hpp"
using mint = soryuusi::modint998244353;

int main() {

  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::Range_Parallel_Unionfind d(n);
  mint ans = 0;
  std::vector<mint> x(n);
  for(auto& v : x){
    int a;
    soryuusi::cin >> a;
    v = a;
  }
  while (q--) {
    int k,a,b; soryuusi::cin >> k >> a >> b;
    d.merge(a,b,k,[&](int i, int j, int ij){
      ans += x[i] * x[j];
      x[ij] = x[i] + x[j];
    });
    soryuusi::cout << ans.val() << '\n';
  }
}
