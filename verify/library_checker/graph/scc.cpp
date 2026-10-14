#include "library/utility/fastio.hpp"
#include<iostream>

#include "library/graph/scc.hpp"

int main(){
  int n,m;
  soryuusi::cin>>n>>m;
  soryuusi::SCC g(n,m);
  while(m--){
    int u,v;
    soryuusi::cin>>u>>v;
    g.add_edge(u,v);
  }
  auto a=g.scc();
  soryuusi::cout<<a.size()<<'\n';
  for(auto&c:a){
    soryuusi::cout<<c.size();
    for(int v:c) soryuusi::cout<<' '<<v;
    soryuusi::cout<<'\n';
  }
}
