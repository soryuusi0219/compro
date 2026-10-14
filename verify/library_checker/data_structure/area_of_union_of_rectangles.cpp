#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/area_union_rectangle.hpp"

int main() {
  int n;
  soryuusi::cin >> n;
  soryuusi::Area_Union_Rectangle<long long> rectangles(n);
  while (n--) {
    long long left, down, right, up;
    soryuusi::cin >> left >> down >> right >> up;
    rectangles.add(left, right, down, up);
  }
  soryuusi::cout << rectangles.area() << '\n';
}
