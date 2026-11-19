#include "library/math/combinatorics.hpp"
#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::modint998244353;
  using FPS = soryuusi::FormalPowerSeries<Mint>;
  Mint r;
  int d;
  soryuusi::cin >> r >> d;
  const auto values = soryuusi::internal::integer_power_table<Mint>(r == 0 ? 1 : d+1,d);
  soryuusi::cout << FPS::sum_of_exponential_times_polynomial_limit(values,r).val() << '\n';
}
