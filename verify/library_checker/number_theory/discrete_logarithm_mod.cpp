#include "library/utility/fastio.hpp"

#include <cstdint>

#include "library/data_structure/hash_table.hpp"
#include "library/math/discrete_logarithm.hpp"
#include "library/math/modint.hpp"

namespace {

using Mint = soryuusi::modint;

Mint power(Mint x, long long n) { return x.pow(n); }
Mint multiply(Mint x, Mint y) { return x*y; }
std::uint64_t hash_mint(Mint x) { return soryuusi::hash_int(x.val()); }

}  // namespace

int main() {
  int t;
  soryuusi::cin >> t;
  while(t--) {
    int x, y, m;
    soryuusi::cin >> x >> y >> m;
    Mint::set_mod(m);
    soryuusi::cout
        << soryuusi::DiscreteLogarithm_Monoid<Mint,Mint,power,multiply,hash_mint>(
               x,1,y,m)
        << '\n';
  }
}
