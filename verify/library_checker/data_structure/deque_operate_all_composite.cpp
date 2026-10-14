#include "library/utility/fastio.hpp"
#include <iostream>
#include <utility>

#include "library/math/modint.hpp"

#include "library/data_structure/swag_deque.hpp"

using mint = soryuusi::modint998244353;
using affine = std::pair<mint, mint>;

affine identity() { return {1, 0}; }

affine compose(affine first, affine second) {
  return {first.first * second.first,
          second.second + second.first * first.second};
}

int main() {

  int q;
  soryuusi::cin >> q;
  soryuusi::swagdq<affine, compose, identity> deque;
  while (q--) {
    int type;
    soryuusi::cin >> type;
    if (type == 0 || type == 1) {
      int a, b;
      soryuusi::cin >> a >> b;
      if (type == 0) deque.push_front({a, b});
      else deque.push_back({a, b});
    } else if (type == 2) {
      deque.pop_front();
    } else if (type == 3) {
      deque.pop_back();
    } else {
      int x;
      soryuusi::cin >> x;
      const auto function = deque.prod();
      soryuusi::cout << (function.first * x + function.second).val() << '\n';
    }
  }
}
