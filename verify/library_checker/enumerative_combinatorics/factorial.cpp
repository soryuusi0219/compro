#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using FPS = soryuusi::FormalPowerSeries<soryuusi::modint998244353>;
  const FPS f{1,1};
  int t;
  soryuusi::cin >> t;
  while(t--){
    int n;
    soryuusi::cin >> n;
    soryuusi::cout << f.prefix_product(n).val() << '\n';
  }
}
