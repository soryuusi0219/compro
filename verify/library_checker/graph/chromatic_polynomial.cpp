#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/graph/chromatic_polynomial.hpp"

#include "library/math/modint.hpp"
using mint = soryuusi::modint998244353;

int main() {

  int n,m;
  soryuusi::cin >> n >> m;
  std::vector<std::vector<int>> G(n);
  while(m--){
    int a,b;
    soryuusi::cin >> a >> b;
    G[a].push_back(b);
    G[b].push_back(a);
  }
  auto p = soryuusi::chromatic_polynomial<mint>(G);
  for(int i = 0; i <= n; ++i) soryuusi::cout << p[i].val() << " \n"[i==n];
}
