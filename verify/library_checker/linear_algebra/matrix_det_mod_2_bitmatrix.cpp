#include "library/utility/fastio.hpp"

#include "library/math/bitmatrix.hpp"

int main() {
  int n;
  soryuusi::cin >> n;
  soryuusi::BitMatrix matrix(n,n);
  std::string row;
  for(int i = 0; i < n; ++i) soryuusi::cin >> row, matrix.set_row(i,row);
  soryuusi::cout << matrix.det() << '\n';
}
