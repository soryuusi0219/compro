#include "library/utility/fastio.hpp"

#include "library/math/matrix.hpp"
#include "library/math/modint.hpp"

int main() {
  using mint = soryuusi::modint998244353;
  int n;
  long long k;
  soryuusi::cin >> n >> k;
  soryuusi::Matrix<mint> matrix(n,n);
  for(auto& row : matrix) for(auto& x : row) soryuusi::cin >> x;
  matrix = matrix.pow(k);
  for(const auto& row : matrix) {
    for(auto x : row) soryuusi::cout << x.val() << ' ';
    soryuusi::cout << '\n';
  }
}
