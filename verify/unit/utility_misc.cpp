#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include "library/utility/rand.hpp"
#include "library/utility/time.hpp"

int main() {
  soryuusi::random_engine.seed(123456789);
  for(int i = 0; i < 1000; ++i) {
    int x = soryuusi::random_integer(-4,7);
    assert(-4 <= x && x <= 7);
  }
  assert(soryuusi::random_integer(5,5) == 5);

  std::ostringstream output;
  auto* old = std::cout.rdbuf(output.rdbuf());
  soryuusi::tick();
  soryuusi::tick();
  std::cout.rdbuf(old);
  const std::string text = output.str();
  assert(std::count(text.begin(),text.end(),'\n') == 2);
  assert(text.find("ms") != std::string::npos);

  std::cout << "utility_misc: all tests passed\n";
}
