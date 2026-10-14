#include "library/utility/fastio.hpp"
#include <iostream>
#include <string>

#include "library/string/lyndon_factorization.hpp"

int main() {

  std::string input;
  soryuusi::cin >> input;
  const auto factors = soryuusi::LyndonFactorization(input);
  for (std::size_t i = 0; i < factors.size(); ++i) {
    soryuusi::cout << factors[i] << " \n"[i + 1 == factors.size()];
  }
}
