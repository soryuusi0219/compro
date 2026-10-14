#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/math/modint.hpp"
#include "library/math/formal_power_series.hpp"
using mint=soryuusi::modint998244353;using FPS=soryuusi::FormalPowerSeries<mint>;using MF=soryuusi::modint_factorials<mint>;
template<class V>void printv(const V&a){for(int i=0;i<(int)a.size();++i)soryuusi::cout<<a[i].val()<<" \n"[i+1==(int)a.size()];}
int main(){int n;soryuusi::cin>>n;MF factorials(n+1);FPS f(n+1);for(int i=0;i<=n;++i)f[i]=factorials.mint_inv_factorial(i+1);auto g=f.inv();for(int i=0;i<=n;++i)g[i]*=factorials.mint_factorial(i);printv(g);}
