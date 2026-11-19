#include <vector>

#include "library/math/modint.hpp"
#include "library/math/number_of_subsequences.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::modint998244353;
  int n;
  soryuusi::cin >> n;
  std::vector<int> values(n);
  for(int& value : values) soryuusi::cin >> value;
  soryuusi::cout << soryuusi::number_of_subsequences<Mint>(values).val() << '\n';
}
