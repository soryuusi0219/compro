#include <cstddef>
#include <cstdint>

#include "library/math/combinatorics.hpp"
#include "library/utility/fastio.hpp"

int main(){
  int n;
  std::uint64_t mod;
  soryuusi::cin >> n >> mod;
  const auto values = soryuusi::montmort_numbers(n,mod);
  for(std::size_t i = 0; i < values.size(); ++i) soryuusi::cout << values[i] << " \n"[i+1 == values.size()];
}
