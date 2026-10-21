#include "library/utility/fastio.hpp"

#include <limits>

#include "library/data_structure/lazy_segtree_template.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::LazySegtreeAssignMin<int> seg(
      n,std::numeric_limits<int>::max());
  while(q--) {
    int type, s, t;
    soryuusi::cin >> type >> s >> t;
    if(type == 0) {
      int x;
      soryuusi::cin >> x;
      seg.assign(s,t+1,x);
    } else {
      soryuusi::cout << seg.prod(s,t+1) << '\n';
    }
  }
}
