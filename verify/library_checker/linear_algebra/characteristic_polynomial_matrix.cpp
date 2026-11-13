#include "library/utility/fastio.hpp"

#include "library/math/matrix_characteristic_polynomial.hpp"
#include "library/math/modint.hpp"

int main() {
  using mint = soryuusi::modint998244353;
  int n;
  soryuusi::cin >> n;
  soryuusi::Matrix<mint> matrix(n,n);
  for(auto& row : matrix) for(auto& x : row) soryuusi::cin >> x;
  for(auto x : soryuusi::characteristic_polynomial(matrix)) soryuusi::cout << x.val() << ' ';
  soryuusi::cout << '\n';
}
