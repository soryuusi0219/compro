#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/tree/heavy_light_decomposition_balanced.hpp"
long long op(long long a,long long b){return a+b;}long long e(){return 0;}
int main(){int n,q;soryuusi::cin>>n>>q;std::vector<long long>a(n);for(auto&x:a)soryuusi::cin>>x;soryuusi::HLdecomposition_Balanced<long long,op,e>h(n);for(int i=1,u,v;i<n;++i){soryuusi::cin>>u>>v;h.add_edge(u,v);}h.build(0,a);while(q--){int t,x,y;soryuusi::cin>>t>>x>>y;if(t)soryuusi::cout<<h.prod(x,y)<<'\n';else h.set(x,h.get(x)+y);}}
