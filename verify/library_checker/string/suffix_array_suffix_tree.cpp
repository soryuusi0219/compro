#include "library/utility/fastio.hpp"

#include <string>

#include "library/string/suffix_tree.hpp"

int main() {
  std::string s;
  soryuusi::cin >> s;
  const soryuusi::SuffixTree<> tree(s);
  const auto& suffix_array = tree.suffix_array();
  for (int i = 0; i < static_cast<int>(suffix_array.size()); ++i) {
    soryuusi::cout << suffix_array[i] << " \n"[i + 1 == static_cast<int>(suffix_array.size())];
  }
}
