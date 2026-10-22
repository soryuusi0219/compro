#include "library/utility/fastio.hpp"

#include "library/math/matrix.hpp"
#include "library/math/modint.hpp"

int main() {
  int n;
  soryuusi::cin >> n;
  soryuusi::Matrix<soryuusi::modint998244353> matrix(n,n);
  for(auto& row : matrix) for(auto& x : row) soryuusi::cin >> x;
  soryuusi::cout << matrix.det().val() << '\n';
}
