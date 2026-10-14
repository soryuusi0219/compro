#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/data_structure/wavelet.hpp"

int main() {

  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> values(n);
  for (int& value : values) soryuusi::cin >> value;
  soryuusi::Wavelet<int> wavelet(values);
  while (q--) {
    int left, right, k;
    soryuusi::cin >> left >> right >> k;
    soryuusi::cout << wavelet.range_kth(left, right, k) << '\n';
  }
}
