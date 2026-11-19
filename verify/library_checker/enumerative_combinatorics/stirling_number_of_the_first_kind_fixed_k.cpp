#include <cstddef>

#include "library/math/combinatorics.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::modint998244353;
  int n, k;
  soryuusi::cin >> n >> k;
  const auto values = soryuusi::stirling_first_kind_fixed_k<Mint>(n, k);
  for(std::size_t i = 0; i < values.size(); ++i) soryuusi::cout << values[i].val() << " \n"[i+1 == values.size()];
}
