#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/static_rectangle_sum.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::StaticRectangleSum<long long> rectangle_sum(n, q);
  while (n--) {
    int x, y;
    long long weight;
    soryuusi::cin >> x >> y >> weight;
    rectangle_sum.add_point(x, y, weight);
  }
  while (q--) {
    int left, down, right, up;
    soryuusi::cin >> left >> down >> right >> up;
    rectangle_sum.add_query(left, right, down, up);
  }
  for (long long answer : rectangle_sum.solve()) soryuusi::cout << answer << '\n';
}
