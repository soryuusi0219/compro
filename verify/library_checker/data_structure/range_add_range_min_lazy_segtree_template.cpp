#include "library/utility/fastio.hpp"
#include <vector>

#include "library/data_structure/lazy_segtree_template.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> a(n);
  for(auto& x : a) soryuusi::cin >> x;
  soryuusi::LazySegtreeAddMin<long long> seg(a);
  while(q--) {
    int t, l, r;
    soryuusi::cin >> t >> l >> r;
    if(t == 0) {
      long long x;
      soryuusi::cin >> x;
      seg.add(l,r,x);
    } else {
      soryuusi::cout << seg.prod(l,r) << '\n';
    }
  }
}
