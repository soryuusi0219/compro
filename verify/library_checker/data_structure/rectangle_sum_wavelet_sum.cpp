#include "library/utility/fastio.hpp"
#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

#include "library/data_structure/wavelet_sum.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<std::tuple<int, int, long long>> points(n);
  for (auto& [x, y, weight] : points) soryuusi::cin >> x >> y >> weight;
  std::sort(points.begin(), points.end());
  std::vector<int> xs(n), ys(n);
  std::vector<long long> weights(n);
  for (int i = 0; i < n; ++i) std::tie(xs[i], ys[i], weights[i]) = points[i];
  soryuusi::Wavelet_Sum<int, long long> wavelet(ys, weights);
  while (q--) {
    int left, down, right, up;
    soryuusi::cin >> left >> down >> right >> up;
    const int first = std::lower_bound(xs.begin(), xs.end(), left) - xs.begin();
    const int last = std::lower_bound(xs.begin(), xs.end(), right) - xs.begin();
    soryuusi::cout << wavelet.sum_interval(first, last, down, up) << '\n';
  }
}
