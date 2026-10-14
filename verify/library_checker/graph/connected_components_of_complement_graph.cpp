#include "library/utility/fastio.hpp"
#include <iostream>
#include "library/graph/complement_graph_connected_components.hpp"
int main(){int n,m;soryuusi::cin>>n>>m;soryuusi::ComplementConnectedComponents g(n,m);while(m--){int u,v;soryuusi::cin>>u>>v;g.add_edge(u,v);}auto a=g.solve();soryuusi::cout<<a.size()<<'\n';for(auto&c:a){soryuusi::cout<<c.size();for(int v:c)soryuusi::cout<<' '<<v;soryuusi::cout<<'\n';}}
