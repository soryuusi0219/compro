#include "library/utility/fastio.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include "library/data_structure/static_range_prod.hpp"

namespace {

int minimum(int x, int y) { return std::min(x,y); }
int infinity() { return std::numeric_limits<int>::max(); }

}  // namespace

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> a(n);
  for(int& x : a) soryuusi::cin >> x;
  soryuusi::StaticRangeProd<int,minimum,infinity> range_minimum(a);
  while(q--) {
    int l, r;
    soryuusi::cin >> l >> r;
    soryuusi::cout << range_minimum.prod(l,r) << '\n';
  }
}
