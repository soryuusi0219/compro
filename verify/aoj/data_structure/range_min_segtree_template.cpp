#include "library/utility/fastio.hpp"

#include "library/data_structure/segtree_template.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::SegtreeMin<unsigned int> seg(n);
  while(q--) {
    int type, x, y;
    soryuusi::cin >> type >> x >> y;
    if(type == 0) seg.set(x,static_cast<unsigned int>(y));
    else soryuusi::cout << seg.prod(x,y+1) << '\n';
  }
}
