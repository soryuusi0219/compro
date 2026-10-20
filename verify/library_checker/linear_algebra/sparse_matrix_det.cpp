#include "library/utility/fastio.hpp"

#include <tuple>
#include <vector>

#include "library/math/determinant_sparse.hpp"
#include "library/math/modint.hpp"

int main() {
  using Mint = soryuusi::modint998244353;
  int n, k;
  soryuusi::cin >> n >> k;
  std::vector<std::tuple<int,int,Mint>> values;
  values.reserve(k);
  while(k--) {
    int row, column, value;
    soryuusi::cin >> row >> column >> value;
    values.emplace_back(row,column,value);
  }
  soryuusi::cout << soryuusi::determinant_sparse(n,std::move(values)).val() << '\n';
}
