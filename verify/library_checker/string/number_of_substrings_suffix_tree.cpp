#include "library/utility/fastio.hpp"

#include <string>

#include "library/string/suffix_tree.hpp"

int main() {
  std::string s;
  soryuusi::cin >> s;
  const soryuusi::SuffixTree<> tree(s);
  soryuusi::cout << tree.distinct_substrings() << '\n';
}
