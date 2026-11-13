#include "library/utility/fastio.hpp"

#include "library/math/matrix.hpp"
#include "library/math/modint.hpp"

int main() {
  using mint = soryuusi::modint998244353;
  int n;
  soryuusi::cin >> n;
  soryuusi::Matrix<mint> matrix(n,n);
  for(auto& row : matrix) for(auto& x : row) soryuusi::cin >> x;
  for(const auto& row : matrix.adjugate()) {
    for(auto x : row) soryuusi::cout << x.val() << ' ';
    soryuusi::cout << '\n';
  }
}
