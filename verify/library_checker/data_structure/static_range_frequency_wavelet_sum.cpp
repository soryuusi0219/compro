#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/data_structure/wavelet_sum.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> values(n);
  for (int& value : values) soryuusi::cin >> value;
  soryuusi::Wavelet_Sum<int, int> wavelet(values, std::vector<int>(n, 1));
  while (q--) {
    int left, right, value;
    soryuusi::cin >> left >> right >> value;
    soryuusi::cout << wavelet.sum_with_val(left, right, value) << '\n';
  }
}
