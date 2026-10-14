#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>
#include <vector>

#include "library/data_structure/lazy_segtree.hpp"

using S = long long;
const S MAX = (1LL << 62);
S op(S x, S y){return std::min(x,y);}
S e(){return MAX;}
using F = long long;
S mapping(F x, S y){return x+y;}
F composition(F x, F y){return x+y;}
F id(){return 0;}


int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<S> a(n);
  for (auto& b : a) {
    soryuusi::cin >> b;
  }
  soryuusi::LazySegtree<S, op, e, F, mapping, composition, id> seg(a);
  while (q--) {
    int type, left, right;
    soryuusi::cin >> type >> left >> right;
    if (type == 0) {
      int x;
      soryuusi::cin >> x;
      seg.apply(left, right, x);
    } else {
      soryuusi::cout << seg.prod(left, right) << '\n';
    }
  }
}
