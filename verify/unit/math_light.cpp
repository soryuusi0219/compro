#include <cassert>
#include <iostream>
#include <vector>

#include "library/math/modint.hpp"
#include "library/math/light/fps_light.hpp"
#include "library/math/light/mint_factorial_light.hpp"

int main() {
  using mint = soryuusi::modint998244353;
  soryuusi::mf_init<mint>(20);
  assert(soryuusi::fact<mint>[5] == mint(120));
  assert(soryuusi::invs<mint>[7]*mint(7) == mint(1));
  assert(soryuusi::nPk<mint>(6,3) == mint(120));
  assert(soryuusi::nCk<mint>(6,3) == mint(20));
  assert(soryuusi::nCk<mint>(3,4) == mint(0));

  using FPS = soryuusi::FPS<mint>;
  FPS f{1,2,3,4};
  FPS inverse = f.inv(4);
  FPS product = f*inverse;
  product.resize(4);
  assert((product == FPS{1,0,0,0}));

  FPS g{0,2,3,4};
  FPS exponential = g.exp(4);
  FPS logarithm = exponential.log(4);
  logarithm.resize(4);
  assert(logarithm == g);
  assert((FPS{2,2,0,0}.pow(3,4) == FPS{8,24,24,8}));

  std::cout << "math_light: all tests passed\n";
}
