#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>
#include <vector>

#include "library/math/modint.hpp"

#include "library/data_structure/range_set_range_prod.hpp"

using mint = soryuusi::modint998244353;
using affine = std::pair<mint, mint>;
affine identity() { return {1, 0}; }
affine compose(affine left, affine right) {
  return {left.first * right.first, right.first * left.second + right.second};
}

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<affine> functions(n);
  for (auto& [a, b] : functions) {
    int av, bv;
    soryuusi::cin >> av >> bv;
    a = av;
    b = bv;
  }
  soryuusi::RangeSetRangeProd<affine, compose, identity> tree(functions);
  while (q--) {
    int type, left, right;
    soryuusi::cin >> type >> left >> right;
    if (type == 0) {
      int a, b;
      soryuusi::cin >> a >> b;
      tree.range_set(left, right, {a, b});
    } else {
      int x;
      soryuusi::cin >> x;
      auto [a, b] = tree.prod(left, right);
      soryuusi::cout << (a * x + b).val() << '\n';
    }
  }
}
