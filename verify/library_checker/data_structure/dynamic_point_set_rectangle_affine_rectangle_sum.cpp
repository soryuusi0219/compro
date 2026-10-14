#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include "library/math/modint.hpp"
#include <tuple>
#include "library/data_structure/lazy_kd_tree.hpp"
using mint=soryuusi::modint998244353;using S=std::pair<mint,mint>;S op(S x,S y){return {x.first+y.first,x.second+y.second};}S e(){return {0,0};}S mapping(S f,S x){return {f.first*x.first+f.second*x.second,x.second};}S composition(S f,S g){return {f.first*g.first,f.first*g.second+f.second};}S id(){return {1,0};}
struct Q{int t,k;long long l,d,r,u;mint w;S f;};int main(){int n,q;soryuusi::cin>>n>>q;std::vector<std::pair<long long,long long>>pts(n);std::vector<S>w(n);for(int i=0,x;i<n;++i){soryuusi::cin>>pts[i].first>>pts[i].second>>x;w[i]={x,1};}std::vector<Q>qs(q);for(auto&z:qs){soryuusi::cin>>z.t;if(z.t==0){long long x,y;int v;soryuusi::cin>>x>>y>>v;z.k=pts.size();z.w=v;pts.push_back({x,y});w.push_back(e());}else if(z.t==1){int v;soryuusi::cin>>z.k>>v;z.w=v;}else if(z.t==2)soryuusi::cin>>z.l>>z.d>>z.r>>z.u;else{int a,b;soryuusi::cin>>z.l>>z.d>>z.r>>z.u>>a>>b;z.f={a,b};}}if(pts.empty()){for(auto z:qs)if(z.t==2)soryuusi::cout<<0<<'\n';return 0;}soryuusi::LazyKDTree<long long,long long,S,op,e,S,mapping,composition,id>tree(pts);tree.build(w);for(auto z:qs){if(z.t<2)tree.set(z.k,{z.w,1});else if(z.t==2)soryuusi::cout<<tree.prod(z.l,z.r,z.d,z.u).first.val()<<'\n';else tree.apply(z.l,z.r,z.d,z.u,z.f);}}
