#include "library/utility/fastio.hpp"
#include <vector>

#include "library/data_structure/segtree_beats_templates.hpp"

using Segtree = soryuusi::SegtreeBeatsChminChmaxAdd<long long>;

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> a(n);
  for(auto& x : a) soryuusi::cin >> x;
  Segtree seg(a);
  while(q--) {
    int t, l, r;
    soryuusi::cin >> t >> l >> r;
    if(t == 3) {
      soryuusi::cout << seg.prod(l,r).sum << '\n';
    } else {
      long long x;
      soryuusi::cin >> x;
      if(t == 0) seg.chmin(l,r,x);
      else if(t == 1) seg.chmax(l,r,x);
      else seg.add(l,r,x);
    }
  }
}
