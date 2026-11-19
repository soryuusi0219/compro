#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::dynamic_modint<0>;
  using FPS = soryuusi::FormalPowerSeries<Mint>;
  Mint::set_mod(998244353);
  int n;
  soryuusi::cin >> n;
  FPS f(n);
  for(auto& x : f) soryuusi::cin >> x;
  auto result = f.compositional_inverse();
  for(int i = 0; i < n; ++i) soryuusi::cout << result[i].val() << " \n"[i+1 == n];
}
