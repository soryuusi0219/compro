#include "library/utility/fastio.hpp"

#include "library/math/matrix.hpp"
#include "library/math/modint.hpp"

int main() {
  using mint = soryuusi::modint998244353;
  int n;
  soryuusi::cin >> n;
  soryuusi::Matrix<mint> matrix(n,n);
  for(auto& row : matrix) for(auto& x : row) soryuusi::cin >> x;
  auto inverse = matrix.try_inv();
  if(!inverse) {
    soryuusi::cout << -1 << '\n';
    return 0;
  }
  for(const auto& row : *inverse) {
    for(auto x : row) soryuusi::cout << x.val() << ' ';
    soryuusi::cout << '\n';
  }
}
