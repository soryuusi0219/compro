#include "library/utility/fastio.hpp"
#include <iostream>
#include <tuple>
#include <vector>
#include "library/graph/dynamic_connectivity_offline_data.hpp"
long long op(long long a,long long b){return a+b;}
int main(){int n,q;soryuusi::cin>>n>>q;std::vector<long long>a(n);for(auto&x:a)soryuusi::cin>>x;int time=0;std::vector<std::tuple<int,int,int,long long>>ops;std::vector<std::vector<int>>asks(q);for(int z=0;z<q;++z){int t,u;soryuusi::cin>>t>>u;if(t<2){int v;soryuusi::cin>>v;ops.emplace_back(time,t,u,v);}else if(t==2){long long x;soryuusi::cin>>x;ops.emplace_back(time,t,u,x);}else asks[time++].push_back(u);}soryuusi::dynamic_connectivity_offline_data<long long,op>dc(a,time);for(auto[t,k,u,v]:ops){if(k==0)dc.add_edge(t,u,v);else if(k==1)dc.erase_edge(t,u,v);else dc.apply(t,u,v);}dc.build();dc.run([&](int t){for(int v:asks[t])soryuusi::cout<<dc.uf.get(v)<<'\n';});}
