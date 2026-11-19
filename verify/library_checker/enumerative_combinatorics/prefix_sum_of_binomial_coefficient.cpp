#include <utility>
#include <vector>

#include "library/math/binomial_coefficient_prefix_sum.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::modint998244353;
  int t;
  soryuusi::cin >> t;
  std::vector<std::pair<int,int>> queries(t);
  for(auto& [n,k] : queries) soryuusi::cin >> n >> k;
  for(Mint x : soryuusi::BinomialCoefficientPrfixSum<Mint>(queries)) soryuusi::cout << x.val() << '\n';
}
