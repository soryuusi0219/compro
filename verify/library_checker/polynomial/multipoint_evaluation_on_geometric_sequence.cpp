#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/math/modint.hpp"
#include "library/math/formal_power_series.hpp"
using mint=soryuusi::modint998244353;
using FPS=soryuusi::FormalPowerSeries<mint>;
std::vector<mint> readv(int n){std::vector<mint> a(n); for(auto& x:a){long long v; soryuusi::cin>>v; x=v;} return a;}
template<class V> void printv(const V& a){for(int i=0;i<(int)a.size();++i) soryuusi::cout<<a[i].val()<<" \n"[i+1==(int)a.size()];}
int main(){int n,m;long long a,r;soryuusi::cin>>n>>m>>a>>r;FPS f(readv(n));printv(f.geo_eval(mint(a),mint(r),m));}
