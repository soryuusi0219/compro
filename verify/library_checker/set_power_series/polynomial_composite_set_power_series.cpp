#include "library/utility/fastio.hpp"
#include <algorithm>
#include <iostream>
#include <vector>
#include "library/math/modint.hpp"
#include "library/math/set_power_series.hpp"
using mint=soryuusi::modint998244353; using SPS=soryuusi::SetPowerSeries<mint>;
std::vector<mint> readv(int n){std::vector<mint>a(n);for(auto&x:a){long long v;soryuusi::cin>>v;x=v;}return a;}
template<class V>void printv(const V&a){for(int i=0;i<(int)a.size();++i)soryuusi::cout<<a[i].val()<<" \n"[i+1==(int)a.size()];}
int main(){int m,n;soryuusi::cin>>m>>n;auto f=readv(m);SPS g(readv(1<<n));printv(soryuusi::composition(f,g));}
