#include "library/utility/fastio.hpp"

#include "library/math/matrix.hpp"
#include "library/math/modint.hpp"

int main() {
  using mint = soryuusi::modint998244353;
  int n,m;
  soryuusi::cin >> n >> m;
  soryuusi::Matrix<mint> matrix(n,m);
  for(auto& row : matrix) for(auto& x : row) soryuusi::cin >> x;
  soryuusi::cout << matrix.rank() << '\n';
}
