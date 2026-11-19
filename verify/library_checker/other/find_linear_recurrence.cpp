#include <utility>

#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using FPS = soryuusi::FormalPowerSeries<soryuusi::modint998244353>;
  int n;
  soryuusi::cin >> n;
  FPS samples(n);
  for(auto& x : samples) soryuusi::cin >> x;
  const auto recurrence = FPS::find_linear_recurrence(std::move(samples));
  soryuusi::cout << recurrence.size() << '\n';
  for(std::size_t i = 0; i < recurrence.size(); ++i){
    if(i) soryuusi::cout << ' ';
    soryuusi::cout << recurrence[i].val();
  }
  soryuusi::cout << '\n';
}
