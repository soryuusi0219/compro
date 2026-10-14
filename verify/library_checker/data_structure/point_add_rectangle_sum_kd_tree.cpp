#include "library/utility/fastio.hpp"
#include <algorithm>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>
#include "library/data_structure/kd_tree.hpp"
using ll=long long;ll op(ll a,ll b){return a+b;}ll e(){return 0;}
struct Q{int type;ll a,b,c,d;};
int main(){int n,q;soryuusi::cin>>n>>q;std::vector<std::pair<ll,ll>>pts(n);std::vector<ll>initial(n);for(int i=0;i<n;++i)soryuusi::cin>>pts[i].first>>pts[i].second>>initial[i];std::vector<Q>qs;qs.reserve(q);for(int i=0;i<q;++i){int t;soryuusi::cin>>t;if(t==0){ll x,y,w;soryuusi::cin>>x>>y>>w;qs.push_back({0,(ll)pts.size(),w,0,0});pts.push_back({x,y});initial.push_back(0);}else{ll l,d,r,u;soryuusi::cin>>l>>d>>r>>u;qs.push_back({1,l,d,r,u});}}auto key=pts;std::sort(key.begin(),key.end());key.erase(std::unique(key.begin(),key.end()),key.end());std::vector<int>id(pts.size());for(int i=0;i<(int)pts.size();++i)id[i]=std::lower_bound(key.begin(),key.end(),pts[i])-key.begin();std::vector<ll>x(key.size()),y(key.size()),w(key.size());for(int i=0;i<(int)key.size();++i){x[i]=key[i].first;y[i]=key[i].second;}for(int i=0;i<n;++i)w[id[i]]+=initial[i];if(key.empty()){for(auto z:qs)if(z.type)soryuusi::cout<<0<<'\n';return 0;} soryuusi::KDTree<ll,ll,ll,op,e>ds(key);ds.build(w);for(auto z:qs){if(z.type==0){int i=id[z.a];ds.set(i,ds.get(i)+z.b);}else soryuusi::cout<<ds.prod(z.a,z.c,z.b,z.d)<<'\n';} }
