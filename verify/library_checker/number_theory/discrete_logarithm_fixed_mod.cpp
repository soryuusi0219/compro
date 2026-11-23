#include <cstdint>

#include "library/math/internal/internal_math.hpp"
#include "library/math/prime_field_tables.hpp"
#include "library/utility/fastio.hpp"

int main(){
  int p,n;
  std::uint32_t g;
  soryuusi::cin >> p >> g >> n;
  const soryuusi::PrimeFieldTables table(p);
  const auto change_of_base = soryuusi::internal::inv_gcd64(table.log(g),p-1).second;
  const soryuusi::internal::barrett exponent_mod(p-1);
  for(int i = 0; i < n; ++i){
    std::uint32_t value;
    soryuusi::cin >> value;
    soryuusi::cout << exponent_mod.mul(table.log(value),change_of_base) << '\n';
  }
}
