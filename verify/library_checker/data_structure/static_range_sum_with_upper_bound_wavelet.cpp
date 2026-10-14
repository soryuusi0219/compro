#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/data_structure/wavelet_sum.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> values(n);
  for (long long& value : values) soryuusi::cin >> value;
  soryuusi::Wavelet_Sum<long long, long long, 60> wavelet(values, values);
  while (q--) {
    int left, right;
    long long upper_bound;
    soryuusi::cin >> left >> right >> upper_bound;
    soryuusi::cout << wavelet.count_lt(left, right, upper_bound + 1) << ' '
              << wavelet.sum_lt(left, right, upper_bound + 1) << '\n';
  }
}
