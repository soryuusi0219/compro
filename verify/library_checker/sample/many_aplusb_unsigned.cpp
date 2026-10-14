#include "library/utility/fastio.hpp"

int main() {
  uint32_t t;
  uint64_t a, b;
  soryuusi::cin >> t;
  while(t--){
    soryuusi::cin >> a >> b;
    soryuusi::cout << a+b << '\n';
  }
}