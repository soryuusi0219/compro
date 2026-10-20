#include "library/utility/fastio.hpp"

#include <string>

#include "library/string/eertree.hpp"

int main() {
  std::string s;
  soryuusi::cin >> s;

  soryuusi::Eertree<> tree(s);
  soryuusi::cout << tree.node_size() - 2 << '\n';
  for (int v = 2; v < tree.node_size(); ++v) {
    soryuusi::cout << tree.parent(v) - 1 << ' '
                   << tree.suffix_link(v) - 1 << '\n';
  }
  for (int i = 0; i < static_cast<int>(s.size()); ++i) {
    soryuusi::cout << tree.maximal_suffix(i) - 1
                   << " \n"[i + 1 == static_cast<int>(s.size())];
  }
}
