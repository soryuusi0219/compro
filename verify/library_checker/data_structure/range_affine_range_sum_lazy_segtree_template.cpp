#include "library/utility/fastio.hpp"
#include <vector>

#include "library/data_structure/lazy_segtree_template.hpp"
#include "library/math/modint.hpp"

int main() {
  using mint = soryuusi::modint998244353;
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<mint> a(n);
  for(auto& x : a) {
    int v;
    soryuusi::cin >> v;
    x = v;
  }
  soryuusi::LazySegtreeAffineSum<mint> seg(a);
  while(q--) {
    int t, l, r;
    soryuusi::cin >> t >> l >> r;
    if(t == 0) {
      int b, c;
      soryuusi::cin >> b >> c;
      seg.affine(l,r,b,c);
    } else {
      soryuusi::cout << seg.prod(l,r).sum.val() << '\n';
    }
  }
}
