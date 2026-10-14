#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/math/modint.hpp"
#include "library/math/formal_power_series.hpp"
using mint=soryuusi::modint998244353;using FPS=soryuusi::FormalPowerSeries<mint>;using MF=soryuusi::modint_factorials<mint>;
template<class V>void printv(const V&a){for(int i=0;i<(int)a.size();++i)soryuusi::cout<<a[i].val()<<" \n"[i+1==(int)a.size()];}
int main(){int n,t;soryuusi::cin>>n>>t;MF factorials(t+1);std::vector<int>c(t);for(int i=0,s;i<n;++i){soryuusi::cin>>s;++c[--s];}FPS f(t+1);for(int i=1;i<=t;++i)if(c[i-1])for(int j=1;i*j<=t;++j)f[i*j]+=factorials.mint_inv(j)*(j&1?c[i-1]:-c[i-1]);auto g=f.exp()>>1;printv(g);}
