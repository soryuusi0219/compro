#include "library/utility/fastio.hpp"

#include <vector>

#include "library/math/modint.hpp"
#include "library/tree/static_top_tree_rerooting.hpp"

using mint = soryuusi::modint998244353;


struct S{
  mint ans, cnt, a, b;
};
struct Srev{
  mint ans, cnt, a, b;
};
S rake(S x, S y){
  return {x.ans + y.ans, x.cnt + y.cnt, x.a, x.b};
}
S comp00(S x, S y) {
  return {x.ans + x.a * y.ans + x.b * y.cnt, x.cnt + y.cnt, x.a * y.a, x.a * y.b + x.b};
}
Srev comp10(Srev x, S y) {
  return {x.ans + x.a * y.ans + x.b * y.cnt, x.cnt + y.cnt, x.a, x.b};
}
Srev comp11(Srev x, Srev y) {
  return {y.ans + y.a * x.ans + y.b * x.cnt, x.cnt + y.cnt, x.a * y.a, x.b * y.a + y.b};
}
S e(){
  return {0,0,1,0};
}
Srev erev(){
  return {0,0,1,0};
}

int main() {
  int n,q; soryuusi::cin>>n>>q;
  std::vector<S> d(2*n-1);
  std::vector<Srev> drev(2*n-1);
  for(int i = 0; i < n; ++i){
    int x; soryuusi::cin>>x;
    d[i] = {x,1,1,0};
    drev[i] = {x,1,1,0};
  }
  soryuusi::StaticTopTreeRerooting<S,Srev,rake,comp00,comp10,comp11,e,erev> STT(2*n-1);
  for(int i = 0; i < n-1; ++i) {
    int u,v,b,c; soryuusi::cin>>u>>v>>b>>c;
    STT.add_edge(u,n+i);
    STT.add_edge(v,n+i);
    d[n+i] = {0,0,b,c};
    drev[n+i] = {0,0,b,c};
  }
  STT.build(0);
  STT.set_data(d, drev);
  while(q--){
    int t,r; soryuusi::cin>>t;
    if(t == 0){
      int w,x; soryuusi::cin>>w>>x>>r;
      STT.set(w,{x,1,1,0},{x,1,1,0});
    }else{
      int e,y,z; soryuusi::cin>>e>>y>>z>>r;
      STT.set(e+n,{0,0,y,z},{0,0,y,z});
    }
    soryuusi::cout<<STT.get(r).ans<<'\n';
  }
}
