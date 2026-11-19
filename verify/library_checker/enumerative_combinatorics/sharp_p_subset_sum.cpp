#include <vector>

#include "library/math/combinatorics.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::modint998244353;
  int n,t;
  soryuusi::cin >> n >> t;
  std::vector<int> values(n);
  for(int& value : values) soryuusi::cin >> value;
  const auto counts = soryuusi::sharp_p_subset_sum<Mint>(values,t);
  for(int i = 1; i <= t; ++i) soryuusi::cout << counts[i].val() << " \n"[i == t];
}
