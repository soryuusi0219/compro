#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/segtree_sparse.hpp"

long long op(long long x, long long y) {return x+y;}
long long identity() {return 0;}

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> a(n);
  soryuusi::Segtree_Sparse<long long,op,identity> seg(n);
  for(int i = 0; i < n; ++i) {
    soryuusi::cin >> a[i];
    seg.add_point(i);
  }
  seg.build();
  for(int i = 0; i < n; ++i) seg.set(i,a[i]);
  while(q--) {
    int type, x, y;
    soryuusi::cin >> type >> x >> y;
    if(type == 0) {
      a[x] += y;
      seg.set(x,a[x]);
    }else{
      soryuusi::cout << seg.prod(x,y) << '\n';
    }
  }
}
