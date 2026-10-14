// https://github.com/soryuusi0219/compro/blob/main/library/graph/chromatic_polynomial.hpp
#include <vector>

#include "library/math/formal_power_series.hpp"
#include "library/math/set_power_series.hpp"

namespace soryuusi {

template<typename T, typename INT>
FormalPowerSeries<T> chromatic_polynomial(const vector<vector<INT>>& G){
  int n = G.size();
  int N = 1 << n;
  SetPowerSeries<T> F(N,1);
  for(int i = 0; i < n; i++) for(auto j : G[i]) F[(1 << i)|(1 << j)] = 0;
  for(int i = 0; i < n; i++) for(int j = N-1; j >= 0; j--){
    j &= ~(1 << i);
    if(F[j] == 0) F[j|(1<<i)] = 0;
  }
  SetPowerSeries<T> e(N,0);
  e[0] = 1;
  auto v = power_projection(F,e,n+1);
  vector<T> x(n+1);
  for(int i = 0; i <= n; i++) x[i] = i;
  return FormalPowerSeries<T>::interpolation(x,v);
}

}  // namespace soryuusi
