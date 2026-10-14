#include "library/utility/fastio.hpp"
#include <iostream>
#include <tuple>
#include <vector>
#include "library/data_structure/lazy_kd_tree.hpp"
using ll=long long;ll op(ll a,ll b){return std::max(a,b);}ll e(){return 0;}ll mapping(ll f,ll x){return f+x;}ll composition(ll f,ll g){return f+g;}ll id(){return 0;}
int main(){int n,q;soryuusi::cin>>n>>q;std::vector<std::tuple<ll,ll,ll,ll,ll>>ops(n+q);soryuusi::LazyKDTree<ll,ll,ll,op,e,ll,mapping,composition,id>tree(q);for(int i=0;i<n;++i){auto&[l,d,r,u,w]=ops[i];soryuusi::cin>>l>>d>>r>>u>>w;}int points=0;for(int i=0;i<q;++i){int t;soryuusi::cin>>t;if(t==0){auto&[l,d,r,u,w]=ops[n+i];soryuusi::cin>>l>>d>>r>>u>>w;}else{ll x,y;soryuusi::cin>>x>>y;ops[n+i]={-1,tree.add_point(x,y),0,0,0};++points;}}if(!points)return 0;tree.build();for(auto[a,b,c,d,w]:ops){if(a==-1)soryuusi::cout<<tree.get(b)<<'\n';else tree.apply(a,c,b,d,w);}}
