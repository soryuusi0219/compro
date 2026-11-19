#include <cstddef>

#include "library/math/combinatorics.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::modint998244353;
  int n;
  soryuusi::cin >> n;
  const auto values = soryuusi::bell_numbers<Mint>(n);
  for(std::size_t i = 0; i < values.size(); ++i) soryuusi::cout << values[i].val() << " \n"[i+1 == values.size()];
}
