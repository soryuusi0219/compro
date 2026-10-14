#include "library/utility/fastio.hpp"
#include <iostream>
#include "library/math/stern_brocot.hpp"
int main(){int q;soryuusi::cin>>q;while(q--){long long n,x,y;soryuusi::cin>>n>>x>>y;auto lo=soryuusi::SB::bisect([&](auto f){return f.first*y>f.second*x;},n).first;auto hi=soryuusi::SB::bisect([&](auto f){return f.first*y>=f.second*x;},n).second;soryuusi::cout<<lo.first<<' '<<lo.second<<' '<<hi.first<<' '<<hi.second<<'\n';}}
