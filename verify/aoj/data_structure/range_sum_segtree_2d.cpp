#include "library/utility/fastio.hpp"

#include "library/data_structure/segtree_2d.hpp"

long long op(long long x, long long y) {return x+y;}
long long identity() {return 0;}

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::Segtree2D<long long,op,identity> seg(n,1);
  while(q--) {
    int type, x, y;
    soryuusi::cin >> type >> x >> y;
    if(type == 0) seg.set(x-1,0,seg.get(x-1,0)+y);
    else soryuusi::cout << seg.prod(x-1,y,0,1) << '\n';
  }
}
