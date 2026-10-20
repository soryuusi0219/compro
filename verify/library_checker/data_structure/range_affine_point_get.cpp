#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/lazy_segtree_template.hpp"
#include "library/math/modint.hpp"

int main() {
  using Mint = soryuusi::modint998244353;
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<Mint> a(n);
  for(Mint& x : a) soryuusi::cin >> x;
  soryuusi::LazySegtreeAffineSum<Mint> segment_tree(a);
  while(q--) {
    int type;
    soryuusi::cin >> type;
    if(type == 0) {
      int l, r, b, c;
      soryuusi::cin >> l >> r >> b >> c;
      segment_tree.affine(l,r,b,c);
    } else {
      int i;
      soryuusi::cin >> i;
      soryuusi::cout << segment_tree.prod(i,i+1).sum.val() << '\n';
    }
  }
}
