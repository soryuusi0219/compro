#include "library/utility/fastio.hpp"

#include "library/math/light/fps_light.hpp"
#include "library/math/modint.hpp"

using mint = soryuusi::modint998244353;
using FPS = soryuusi::FPS<mint>;

int main() {
  int n;
  soryuusi::cin >> n;
  FPS f(n);
  for(auto& x : f) soryuusi::cin >> x;
  const auto answer = f.exp(n);
  for(int i = 0; i < n; ++i) soryuusi::cout << answer[i].val() << " \n"[i+1 == n];
}
