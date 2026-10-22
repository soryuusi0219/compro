#include "library/utility/fastio.hpp"

#include <string>

#include "library/string/trie.hpp"

int main() {
  int q;
  soryuusi::cin >> q;
  soryuusi::Trie<char,'A','T'> trie;
  while(q--) {
    std::string command, word;
    soryuusi::cin >> command >> word;
    if(command[0] == 'i') trie.insert(word);
    else soryuusi::cout << (trie.count(word) ? "yes\n" : "no\n");
  }
}
