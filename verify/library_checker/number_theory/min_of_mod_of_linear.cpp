#include "library/utility/fastio.hpp"

#include "library/math/min_mod_linear.hpp"

int main() {
  int t;
  soryuusi::cin >> t;
  while(t--) {
    long long n, m, a, b;
    soryuusi::cin >> n >> m >> a >> b;
    soryuusi::cout << soryuusi::MinModLinear(a,b,m,0LL,n).second << '\n';
  }
}
