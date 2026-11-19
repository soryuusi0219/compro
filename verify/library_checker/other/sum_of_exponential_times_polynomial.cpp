#include <algorithm>
#include <utility>

#include "library/math/combinatorics.hpp"
#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::modint998244353;
  using FPS = soryuusi::FormalPowerSeries<Mint>;
  Mint r;
  int d;
  long long n;
  soryuusi::cin >> r >> d >> n;
  const int count = r == 0 ? (n != 0) : std::min<long long>(d+1,n);
  auto values = soryuusi::internal::integer_power_table<Mint>(count,d);
  soryuusi::cout << FPS::sum_of_exponential_times_polynomial(std::move(values),r,n).val() << '\n';
}
