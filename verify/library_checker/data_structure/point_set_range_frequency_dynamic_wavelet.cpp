#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/data_structure/dynamic_wavelet.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> values(n);
  for (int& value : values) soryuusi::cin >> value;
  soryuusi::DynamicWavelet<int> wavelet(values);
  while (q--) {
    int type;
    soryuusi::cin >> type;
    if (type == 0) {
      int index, value;
      soryuusi::cin >> index >> value;
      wavelet.set(index, value);
    } else {
      int left, right, value;
      soryuusi::cin >> left >> right >> value;
      soryuusi::cout << wavelet.rank(left, right, value) << '\n';
    }
  }
}
