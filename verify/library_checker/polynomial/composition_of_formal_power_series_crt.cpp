#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::dynamic_modint<0>;
  using FPS = soryuusi::FormalPowerSeries<Mint>;
  Mint::set_mod(998244353);
  int n;
  soryuusi::cin >> n;
  FPS f(n), g(n);
  for(auto& x : f) soryuusi::cin >> x;
  for(auto& x : g) soryuusi::cin >> x;
  auto result = f.composition(g);
  for(int i = 0; i < n; ++i) soryuusi::cout << result[i].val() << " \n"[i+1 == n];
}
