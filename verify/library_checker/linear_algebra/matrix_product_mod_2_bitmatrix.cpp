#include "library/utility/fastio.hpp"

#include "library/math/bitmatrix.hpp"

int main() {
  int n,m,k;
  soryuusi::cin >> n >> m >> k;
  soryuusi::BitMatrix a(n,m), b(m,k);
  std::string row;
  for(int i = 0; i < n; ++i) soryuusi::cin >> row, a.set_row(i,row);
  for(int i = 0; i < m; ++i) soryuusi::cin >> row, b.set_row(i,row);
  auto c = a*b;
  for(int i = 0; i < n; ++i) soryuusi::cout << c.row_string(i) << '\n';
}
