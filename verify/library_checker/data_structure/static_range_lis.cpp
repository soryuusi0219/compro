#include <vector>

#include "library/utility/fastio.hpp"
#include "library/data_structure/static_range_lis.hpp"

int main() {
  int n,q;
  soryuusi::cin >> n >> q;
  std::vector<int> values(n);
  for(auto& x : values) soryuusi::cin >> x;
  soryuusi::StaticRangeLIS<int> lis(values);
  while(q--) {
    int l,r;
    soryuusi::cin >> l >> r;
    soryuusi::cout << lis.query(l,r) << '\n';
  }
}
