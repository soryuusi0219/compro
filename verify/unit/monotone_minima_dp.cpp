#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

#include "library/utility/monotone_minima_dp.hpp"

int main() {
  constexpr int n = 50;
  auto cost = [](int i, int j){return (j-i)*(j-i);};
  auto result = soryuusi::monotoneminima_dp<int>(n,cost);
  std::vector<int> expected(n+1,std::numeric_limits<int>::max());
  expected[0] = 0;
  for(int r = 1; r <= n; ++r) {
    for(int l = 0; l < r; ++l) expected[r] = std::min(expected[r],expected[l]+cost(l,r));
  }
  assert(result == expected);
  std::cout << "monotone_minima_dp: all tests passed\n";
}
