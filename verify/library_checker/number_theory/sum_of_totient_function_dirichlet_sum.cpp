#include "library/utility/fastio.hpp"

#include <cmath>

#include "library/math/dirichlet_sum.hpp"
#include "library/math/modint.hpp"

int main() {
  long long n;
  soryuusi::cin >> n;
  long long k = std::sqrt((long double)n);
  while((k+1)*(k+1) <= n) ++k;
  while(k*k > n) --k;
  using Sum = soryuusi::Dirichlet_Sum<soryuusi::modint998244353,1>;
  Sum::init(n,k);
  soryuusi::cout << (Sum::zetashift(1)/Sum::zeta()).getN().val() << '\n';
}
