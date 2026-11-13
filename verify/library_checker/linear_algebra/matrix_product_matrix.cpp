#include "library/utility/fastio.hpp"

#include "library/math/matrix.hpp"
#include "library/math/modint.hpp"

int main() {
  using mint = soryuusi::modint998244353;
  int n,m,k;
  soryuusi::cin >> n >> m >> k;
  soryuusi::Matrix<mint> a(n,m), b(m,k);
  for(auto& row : a) for(auto& x : row) soryuusi::cin >> x;
  for(auto& row : b) for(auto& x : row) soryuusi::cin >> x;
  auto c = a*b;
  for(const auto& row : c) {
    for(auto x : row) soryuusi::cout << x.val() << ' ';
    soryuusi::cout << '\n';
  }
}
