#include "library/utility/fastio.hpp"

#include "library/math/matrix_det_arbitrary_mod.hpp"

int main() {
  int n,mod;
  soryuusi::cin >> n >> mod;
  soryuusi::Matrix<int> matrix(n,n);
  for(auto& row : matrix) for(auto& x : row) soryuusi::cin >> x;
  soryuusi::cout << soryuusi::matrix_det_arbitrary_mod(matrix,mod) << '\n';
}
