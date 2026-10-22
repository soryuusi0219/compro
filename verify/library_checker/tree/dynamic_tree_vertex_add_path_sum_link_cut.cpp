#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/link_cut.hpp"

long long op(long long x, long long y) { return x+y; }
long long identity() { return 0; }

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::linkcuttree<long long,op,identity> tree(n);
  for(int i = 0; i < n; ++i) {
    long long value;
    soryuusi::cin >> value;
    tree.set(i,value);
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
      int p;
      long long x;
      soryuusi::cin >> p >> x;
      tree.set(p,tree.get(p)+x);
    } else {
      int u, v;
      soryuusi::cin >> u >> v;
      soryuusi::cout << tree.prod(u,v) << '\n';
    }
  }
}
