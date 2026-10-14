#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/math/modint.hpp"
#include "library/math/formal_power_series.hpp"
using mint=soryuusi::modint998244353;using FPS=soryuusi::FormalPowerSeries<mint>;
template<class V>void printv(const V&a){for(int i=0;i<(int)a.size();++i)soryuusi::cout<<a[i].val()<<" \n"[i+1==(int)a.size()];}
int main(){int n;soryuusi::cin>>n;FPS f(n+1);for(long long i=0;i*(3*i-1)/2<=n;++i)f[i*(3*i-1)/2]=(i&1?-1:1);for(long long i=1;i*(3*i+1)/2<=n;++i)f[i*(3*i+1)/2]=(i&1?-1:1);printv(f.inv());}
