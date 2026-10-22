#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/bit_sparse.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> a(n);
  soryuusi::BIT_Sparse<long long> bit(n);
  for(int i = 0; i < n; ++i) {
    soryuusi::cin >> a[i];
    bit.add_point(i);
  }
  bit.build();
  for(int i = 0; i < n; ++i) bit.add(i,a[i]);
  while(q--) {
    int type, x, y;
    soryuusi::cin >> type >> x >> y;
    if(type == 0) bit.add(x,y);
    else soryuusi::cout << bit.sum(x,y) << '\n';
  }
}
