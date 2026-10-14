#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/math/modint.hpp"
#include "library/graph/enumerate_triangles.hpp"
using mint=soryuusi::modint998244353;
int main(){int n,m;soryuusi::cin>>n>>m;std::vector<mint>w(n);for(auto&x:w){int v;soryuusi::cin>>v;x=v;}soryuusi::EnumerateTriangles g(n,m);while(m--){int u,v;soryuusi::cin>>u>>v;g.add_edge(u,v);}mint ans=0;g.solve([&](int i,int j,int k){ans+=w[i]*w[j]*w[k];});soryuusi::cout<<ans.val()<<'\n';}
