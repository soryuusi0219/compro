#include "library/utility/fastio.hpp"

#include "library/data_structure/bit_2d.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::BIT2D<long long> bit(n,1);
  while(q--) {
    int type, x, y;
    soryuusi::cin >> type >> x >> y;
    if(type == 0) bit.add(x-1,0,y);
    else soryuusi::cout << bit.sum(x-1,y,0,1) << '\n';
  }
}
