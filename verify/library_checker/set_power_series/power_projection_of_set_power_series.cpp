#include "library/utility/fastio.hpp"
#include <algorithm>
#include <iostream>
#include <vector>
#include "library/math/modint.hpp"
#include "library/math/set_power_series.hpp"
using mint=soryuusi::modint998244353; using SPS=soryuusi::SetPowerSeries<mint>;
std::vector<mint> readv(int n){std::vector<mint>a(n);for(auto&x:a){long long v;soryuusi::cin>>v;x=v;}return a;}
template<class V>void printv(const V&a){for(int i=0;i<(int)a.size();++i)soryuusi::cout<<a[i].val()<<" \n"[i+1==(int)a.size()];}
int main(){int n,m;soryuusi::cin>>n>>m;SPS f(readv(1<<n)),g(readv(1<<n));std::reverse(g.begin(),g.end());printv(soryuusi::power_projection(f,g,m));}
