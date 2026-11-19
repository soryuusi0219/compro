#include <vector>

#include "library/math/combinatorics.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::modint998244353;
  int t;
  soryuusi::cin >> t;
  std::vector<int> queries(t);
  for(int& n : queries) soryuusi::cin >> n;
  for(Mint value : soryuusi::many_factorials<Mint>(queries)) soryuusi::cout << value.val() << '\n';
}
