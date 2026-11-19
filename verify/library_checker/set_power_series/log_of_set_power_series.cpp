#include "library/math/modint.hpp"
#include "library/math/set_power_series.hpp"
#include "library/utility/fastio.hpp"

int main(){
  int n;
  soryuusi::cin >> n;
  soryuusi::SetPowerSeries<soryuusi::modint998244353> f(1<<n,0);
  for(auto& x : f) soryuusi::cin >> x;
  auto g = f.log();
  for(int i = 0; i < (1<<n); ++i) soryuusi::cout << g[i].val() << " \n"[i+1 == (1<<n)];
}
