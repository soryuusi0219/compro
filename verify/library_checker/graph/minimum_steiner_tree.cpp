#include "library/utility/fastio.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>
#include "library/tree/steiner_tree.hpp"
int main(){int n,m;soryuusi::cin>>n>>m;std::vector<std::vector<std::pair<int,long long>>>g(n);std::vector<std::vector<int>>idx(n,std::vector<int>(n,-1));std::vector<std::vector<long long>>best(n,std::vector<long long>(n,std::numeric_limits<long long>::max()));for(int i=0,u,v,w;i<m;++i){soryuusi::cin>>u>>v>>w;g[u].push_back({v,w});g[v].push_back({u,w});if(w<best[u][v])best[u][v]=best[v][u]=w,idx[u][v]=idx[v][u]=i;}int k;soryuusi::cin>>k;std::vector<int>term(k);for(auto&x:term)soryuusi::cin>>x;auto[cost,edges]=soryuusi::steiner_tree<long long,0,(1LL<<60)>(g,term);soryuusi::cout<<cost<<' '<<edges.size()<<'\n';for(auto[u,v,w]:edges)soryuusi::cout<<idx[u][v]<<' ';soryuusi::cout<<'\n';}
