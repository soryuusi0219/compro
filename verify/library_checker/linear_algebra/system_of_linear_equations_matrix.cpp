#include "library/utility/fastio.hpp"

#include "library/math/matrix.hpp"
#include "library/math/modint.hpp"

int main() {
  using mint = soryuusi::modint998244353;
  int n,m;
  soryuusi::cin >> n >> m;
  soryuusi::Matrix<mint> matrix(n,m);
  for(auto& row : matrix) for(auto& x : row) soryuusi::cin >> x;
  std::vector<mint> b(n);
  for(auto& x : b) soryuusi::cin >> x;
  auto result = matrix.solve(b);
  if(!result.has_solution) {
    soryuusi::cout << -1 << '\n';
    return 0;
  }
  soryuusi::cout << result.nullspace_basis.size() << '\n';
  for(auto x : result.particular_solution) soryuusi::cout << x.val() << ' ';
  soryuusi::cout << '\n';
  for(const auto& basis : result.nullspace_basis) {
    for(auto x : basis) soryuusi::cout << x.val() << ' ';
    soryuusi::cout << '\n';
  }
}
