#include "library/utility/fastio.hpp"

#include "library/math/bitmatrix.hpp"

int main() {
  int n,m;
  soryuusi::cin >> n >> m;
  soryuusi::BitMatrix matrix(n,m);
  std::string row;
  for(int i = 0; i < n; ++i) soryuusi::cin >> row, matrix.set_row(i,row);
  soryuusi::cin >> row;
  std::vector<bool> b(n);
  for(int i = 0; i < n; ++i) b[i] = row[i]-'0';
  auto result = matrix.solve(b);
  if(!result.has_solution) {
    soryuusi::cout << -1 << '\n';
    return 0;
  }
  soryuusi::cout << result.nullspace_basis.size() << '\n';
  for(bool x : result.particular_solution) soryuusi::cout << int(x);
  soryuusi::cout << '\n';
  for(const auto& basis : result.nullspace_basis) {
    for(bool x : basis) soryuusi::cout << int(x);
    soryuusi::cout << '\n';
  }
}
