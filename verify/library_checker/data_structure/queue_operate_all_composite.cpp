#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>

#include "library/math/modint.hpp"

#include "library/data_structure/swag.hpp"

using mint = soryuusi::modint998244353;
using affine = std::pair<mint, mint>;

affine identity() { return {1, 0}; }

affine compose(affine first, affine second) {
  return {first.first * second.first,
          first.second + first.first * second.second};
}

int main() {

  int q;
  soryuusi::cin >> q;
  soryuusi::swag<affine, compose, identity> queue;
  while (q--) {
    int type;
    soryuusi::cin >> type;
    if (type == 0) {
      int a, b;
      soryuusi::cin >> a >> b;
      queue.push({a, b});
    } else if (type == 1) {
      queue.pop();
    } else {
      int x;
      soryuusi::cin >> x;
      const auto function = queue.prod();
      soryuusi::cout << (function.first * x + function.second).val() << '\n';
    }
  }
}
