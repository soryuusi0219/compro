#include "library/utility/fastio.hpp"

#include <utility>
#include <vector>

#include "library/data_structure/static_rmq.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> a(n);
  for(int& x : a) soryuusi::cin >> x;
  const soryuusi::StaticRMQ<int,false,64> range_minimum(std::move(a));
  while(q--) {
    int l, r;
    soryuusi::cin >> l >> r;
    soryuusi::cout << range_minimum.prod(l,r) << '\n';
  }
}
