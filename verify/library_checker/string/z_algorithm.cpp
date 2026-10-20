#include "library/utility/fastio.hpp"

#include <string>

#include "library/string/z_algorithm.hpp"

int main() {
  std::string s;
  soryuusi::cin >> s;
  const auto z = soryuusi::z_algo(s);
  for (int i = 0; i < static_cast<int>(z.size()); ++i) {
    soryuusi::cout << z[i] << " \n"[i + 1 == static_cast<int>(z.size())];
  }
}
