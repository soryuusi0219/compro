#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/math/modint.hpp"

#include "library/data_structure/static_rectangle_add_rectangle_sum.hpp"

using mint = soryuusi::modint998244353;

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  soryuusi::StaticRectangleAddRectangleSum<mint> solver(n, q);
  while (n--) {
    int left, down, right, up, weight;
    soryuusi::cin >> left >> down >> right >> up >> weight;
    solver.add_rectangle(left, right, down, up, weight);
  }
  while (q--) {
    int left, down, right, up;
    soryuusi::cin >> left >> down >> right >> up;
    solver.add_query(left, right, down, up);
  }
  for (mint answer : solver.solve()) soryuusi::cout << answer.val() << '\n';
}
