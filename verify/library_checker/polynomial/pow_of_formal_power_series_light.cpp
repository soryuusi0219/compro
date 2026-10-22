#include "library/utility/fastio.hpp"

#include <algorithm>

#include "library/math/light/fps_light.hpp"
#include "library/math/modint.hpp"

using mint = soryuusi::modint998244353;
using FPS = soryuusi::FPS<mint>;

int main() {
  int n;
  long long k;
  soryuusi::cin >> n >> k;
  FPS f(n);
  for(auto& x : f) soryuusi::cin >> x;
  FPS answer(n);
  if(k == 0) {
    answer[0] = 1;
  }else{
    int first = 0;
    while(first < n && f[first] == mint(0)) ++first;
    if(first < n && first <= (n-1)/k) {
      FPS g(f.begin()+first,f.end());
      const int shift = (long long)first*k;
      auto power = g.pow(k,n-shift);
      std::copy(power.begin(),power.end(),answer.begin()+shift);
    }
  }
  for(int i = 0; i < n; ++i) soryuusi::cout << answer[i].val() << " \n"[i+1 == n];
}
