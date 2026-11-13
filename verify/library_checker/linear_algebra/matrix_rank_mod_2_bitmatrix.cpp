#include "library/utility/fastio.hpp"

#include "library/math/bitmatrix.hpp"

int main() {
  int n,m;
  soryuusi::cin >> n >> m;
  soryuusi::BitMatrix matrix(n,m);
  std::string row;
  for(int i = 0; i < n; ++i) soryuusi::cin >> row, matrix.set_row(i,row);
  soryuusi::cout << matrix.rank() << '\n';
}
