#include "library/utility/fastio.hpp"

#include <string>

#include "library/string/suffix_tree.hpp"

int main() {
  std::string s, t;
  soryuusi::cin >> s >> t;
  soryuusi::SuffixTree<> tree(s);
  const auto [a, c, length] = tree.longest_common_substring(t);
  soryuusi::cout << a << ' ' << a + length << ' '
                 << c << ' ' << c + length << '\n';
}
