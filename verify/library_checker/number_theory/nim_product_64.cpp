#include "library/utility/fastio.hpp"

#include <cstdint>

#include "library/math/nimber.hpp"

int main() {
  int t;
  soryuusi::cin >> t;
  while(t--) {
    std::uint64_t a, b;
    soryuusi::cin >> a >> b;
    soryuusi::cout << (soryuusi::Nimber(a)*soryuusi::Nimber(b)).get() << '\n';
  }
}
