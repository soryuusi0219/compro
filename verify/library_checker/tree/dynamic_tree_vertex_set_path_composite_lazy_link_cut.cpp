#include "library/utility/fastio.hpp"

#include <vector>

#include "library/math/modint.hpp"
#include "library/data_structure/lazy_link_cut.hpp"
using mint = soryuusi::modint998244353;
struct S{mint a, b;};
S op(S x, S y) { return {x.a*y.a,y.a*x.b+y.b}; }
S identity() { return {0,0}; }
using F = bool;
S mp(bool, S x) { return x;}
bool cp(bool, bool) { return false; }
bool id() { return false; }

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::lazy_linkcuttree<S,op,identity,F,mp,cp,id> tree(n);
  for(int i = 0; i < n; ++i) {
    int a,b;
    soryuusi::cin >> a >> b;
    tree.set(i,{a,b});
  }
  for(int i = 1; i < n; ++i) {
    int u, v;
    soryuusi::cin >> u >> v;
    tree.link(u,v);
  }
  while(q--) {
    int type;
    soryuusi::cin >> type;
    if(type == 0) {
      int u, v, w, x;
      soryuusi::cin >> u >> v >> w >> x;
      tree.cut(u,v);
      tree.link(w,x);
    } else if(type == 1) {
      int p, c, d;
      soryuusi::cin >> p >> c >> d;
      tree.set(p,{c,d});
    } else {
      int u, v, x;
      soryuusi::cin >> u >> v >> x;
      S ans = tree.prod(u,v);
      soryuusi::cout << (ans.a * x + ans.b) << '\n';
    }
  }
}
