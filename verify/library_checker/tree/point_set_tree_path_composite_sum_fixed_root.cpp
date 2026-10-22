#include "library/utility/fastio.hpp"

#include <vector>

#include "library/math/modint.hpp"
#include "library/tree/static_top_tree.hpp"

using mint = soryuusi::modint998244353;

struct S{
  mint ans, cnt, a, b;
};
S rake(S x, S y){
  return {x.ans + y.ans, x.cnt + y.cnt, x.a, x.b};
}
S comp(S x, S y) {
  return {x.ans + x.a * y.ans + x.b * y.cnt, x.cnt + y.cnt, x.a * y.a, x.a * y.b + x.b};
}

int main() {
  int n,q; soryuusi::cin>>n>>q;
  std::vector<int> a(n);
  for(auto& x : a) soryuusi::cin>>x;
  soryuusi::StaticTopTree<S,rake,comp> STT(n);
  std::vector<std::pair<int,int>> es(n-1);
  std::vector<std::pair<mint,mint>> edata(n-1);
  for(int i = 0; i < n-1; ++i) {
    int u,v,b,c; soryuusi::cin>>u>>v>>b>>c;
    es[i] = {u,v};
    edata[i] = {b,c};
    STT.add_edge(u,v);
  }
  STT.build(0);
  soryuusi::vector<S> d(n, S{0,1,1,0});
  for(int i = 0; i < n-1; ++i) {
    auto& [u,v] = es[i];
    if(STT.parent(u) != v) std::swap(u,v);
    d[u].a = edata[i].first;
    d[u].b = edata[i].second;
  }
  for(int i = 0; i < n; ++i) d[i].ans = a[i] * d[i].a + d[i].b;
  STT.set_data(d);
  while(q--){
    int t; soryuusi::cin>>t;
    if(t == 0){
      int w,x; soryuusi::cin>>w>>x;
      a[w] = x;
      d[w].ans = x * d[w].a + d[w].b;
      STT.set(w,d[w]);
    }else{
      int e,y,z; soryuusi::cin>>e>>y>>z;
      int u = es[e].first;
      d[u].a = y;
      d[u].b = z;
      d[u].ans = a[u] * d[u].a + d[u].b;
      STT.set(u,d[u]);
    }
    soryuusi::cout<<STT.get().ans<<'\n';
  }
}
