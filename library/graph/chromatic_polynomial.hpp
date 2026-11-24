// https://github.com/soryuusi0219/compro/blob/main/library/graph/chromatic_polynomial.hpp
#pragma once
#include <vector>

#include "../math/formal_power_series.hpp"
#include "../math/internal/internal_bit.hpp"
#include "../math/set_power_series.hpp"

namespace soryuusi {

template<typename T, typename INT>
FormalPowerSeries<T> chromatic_polynomial(const vector<vector<INT>>& G){
  int n = G.size();
  int N = 1 << n;
  vector<unsigned> adjacent(n);
  for(int i = 0; i < n; ++i) for(int j : G[i]){
    adjacent[i] |= 1U<<j;
    adjacent[j] |= 1U<<i;
  }
  SetPowerSeries<T> F(N,0);
  F[0] = 1;
  for(unsigned mask = 1; mask < static_cast<unsigned>(N); ++mask){
    const int v = soryuusi::countr_zero(mask);
    if(!(adjacent[v]&mask)) F[mask] = F[mask&(mask-1)];
  }
  SetPowerSeries<T> e(N,0);
  e[0] = 1;
  auto v = power_projection(F,e,n+1);
  vector<T> x(n+1);
  for(int i = 0; i <= n; i++) x[i] = i;
  return FormalPowerSeries<T>::interpolation(x,v);
}

}  // namespace soryuusi
