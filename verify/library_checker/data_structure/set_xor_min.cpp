#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/data_structure/binary_trie.hpp"

int main() {

  int q;
  soryuusi::cin >> q;
  soryuusi::Binary_Trie<int, 30> trie;
  while (q--) {
    int type, x;
    soryuusi::cin >> type >> x;
    if (type == 0) {
      trie.insert(x);
    } else if (type == 1) {
      trie.erase(x);
    } else {
      trie.operate_xor(x);
      soryuusi::cout << trie.min_element() << '\n';
      trie.operate_xor(x);
    }
  }
}
