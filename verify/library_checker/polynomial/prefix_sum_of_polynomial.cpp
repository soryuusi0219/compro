#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using FPS = soryuusi::FormalPowerSeries<soryuusi::modint998244353>;
  int n;
  soryuusi::cin >> n;
  FPS f(n);
  for(auto& x : f) soryuusi::cin >> x;
  const auto sums = f.prefix_sum();
  for(int i = 0; i <= n; ++i) soryuusi::cout << sums[i].val() << " \n"[i == n];
}
