#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/data_structure/static_range_inversions.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> values(n);
  for (int& value : values) soryuusi::cin >> value;
  soryuusi::StaticRangeInversions<int> inversions(values, q);
  while (q--) {
    int left, right;
    soryuusi::cin >> left >> right;
    soryuusi::cout << inversions.query(left, right) << '\n';
  }
}
