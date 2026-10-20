#include "library/utility/fastio.hpp"

#include <string>

#include "library/string/manacher.hpp"

int main() {
  std::string s;
  soryuusi::cin >> s;
  const auto lengths = soryuusi::Manacher(s);
  for (int i = 0; i < static_cast<int>(lengths.size()); ++i) {
    soryuusi::cout << lengths[i]
                   << " \n"[i + 1 == static_cast<int>(lengths.size())];
  }
}
