#include "library/utility/fastio.hpp"

#include <vector>

#include "library/math/convolution_subset.hpp"
#include "library/math/modint.hpp"

int main() {
  using Mint = soryuusi::modint998244353;
  int n;
  soryuusi::cin >> n;
  const int size = 1 << n;
  std::vector<Mint> a(size), b(size);
  for(Mint& x : a) soryuusi::cin >> x;
  for(Mint& x : b) soryuusi::cin >> x;
  const auto c = soryuusi::convolution_subset(a,b);
  for(int i = 0; i < size; ++i) {
    soryuusi::cout << c[i].val() << " \n"[i+1 == size];
  }
}
