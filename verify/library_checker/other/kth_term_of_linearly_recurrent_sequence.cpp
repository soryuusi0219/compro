#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using FPS = soryuusi::FormalPowerSeries<soryuusi::modint998244353>;
  int d;
  long long k;
  soryuusi::cin >> d >> k;
  FPS initial(d), recurrence(d);
  for(auto& x : initial) soryuusi::cin >> x;
  for(auto& x : recurrence) soryuusi::cin >> x;
  soryuusi::cout << FPS::kth_term_of_linear_recurrence(initial,recurrence,k).val() << '\n';
}
