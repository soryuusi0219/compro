#include <utility>

#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using FPS = soryuusi::FormalPowerSeries<soryuusi::modint998244353>;
  int d;
  long long k;
  soryuusi::cin >> d >> k;
  FPS f(d),g(d+1);
  for(auto& x : f) soryuusi::cin >> x;
  g[0] = 1;
  for(int i = 1; i <= d; ++i){
    soryuusi::cin >> g[i];
    g[i] = -g[i];
  }
  (f *= g).pred(d);
  soryuusi::cout << FPS::Bostan_Mori(std::move(f),std::move(g),k).val() << '\n';
}
