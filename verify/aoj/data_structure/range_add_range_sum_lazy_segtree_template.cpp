#include "library/utility/fastio.hpp"

#include "library/data_structure/lazy_segtree_template.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::LazySegtreeAddSum<long long> seg(n);
  while(q--) {
    int type, s, t;
    soryuusi::cin >> type >> s >> t;
    --s;
    if(type == 0) {
      long long x;
      soryuusi::cin >> x;
      seg.add(s,t,x);
    } else {
      soryuusi::cout << seg.prod(s,t).sum << '\n';
    }
  }
}
