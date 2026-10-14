#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>
#include "library/math/modint.hpp"
#include "library/graph/enumerate_cliques.hpp"
using mint=soryuusi::modint998244353;
int main(){int n,m;soryuusi::cin>>n>>m;std::vector<mint>w(n);for(auto&x:w){int v;soryuusi::cin>>v;x=v;} soryuusi::EnumerateCliques<100> g(n);while(m--){int u,v;soryuusi::cin>>u>>v;g.add_edge(u,v);}soryuusi::cout<<g.weighted_sum(w).val()<<'\n';}
