#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using FPS = soryuusi::FormalPowerSeries<soryuusi::modint998244353>;
  int d,m;
  long long k;
  soryuusi::cin >> d >> k >> m;
  FPS initial(d), recurrence(d);
  for(auto& x : initial) soryuusi::cin >> x;
  for(auto& x : recurrence) soryuusi::cin >> x;
  const auto result = FPS::linear_recurrence_terms(initial,recurrence,k,m);
  for(int i = 0; i < m; ++i) soryuusi::cout << result[i].val() << " \n"[i+1 == m];
}
