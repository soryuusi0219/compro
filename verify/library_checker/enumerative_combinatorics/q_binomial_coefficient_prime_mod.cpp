#include <algorithm>
#include <utility>
#include <vector>

#include "library/math/modint.hpp"
#include "library/math/modint_q_factorials.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::dynamic_modint<0>;
  int t, p, q;
  soryuusi::cin >> t >> p >> q;
  Mint::set_mod(p);
  std::vector<std::pair<int,int>> queries(t);
  int limit = 0;
  for(auto& [n,k] : queries){
    soryuusi::cin >> n >> k;
    if(0 < k && k < n) limit = std::max(limit,n);
  }
  soryuusi::modint_q_factorials<Mint> table(Mint(q),limit);
  for(auto [n,k] : queries) soryuusi::cout << table.nCk(n,k).val() << '\n';
}
