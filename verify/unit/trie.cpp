#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "library/string/trie.hpp"

int main() {
  soryuusi::Trie<char> trie;
  for(const std::string s : {"", "a", "ab", "ab", "b"}) trie.insert(s);
  assert(trie.size() == 5);
  assert(trie.count("ab") == 2 && trie.count("ac") == 0);
  assert(trie.prefix_count("a") == 3);
  assert(trie.kth(0) == "" && trie.kth(2) == "ab" && trie.max_string() == "b");
  assert(trie.longest_prefix("abc") == 2);
  assert(trie.lcp("ac") == 1 && trie.node_index_of("ab") >= 0);

  soryuusi::Trie<int,0,3> integer_trie;
  integer_trie.insert(std::vector<int>{1,2});
  integer_trie.insert(std::vector<int>{1,3});
  assert((integer_trie.kth(1) == std::vector<int>{1,3}));
  assert(integer_trie.lcp(std::vector<int>{1,0}) == 1);
  assert(integer_trie.node_index_of(std::vector<int>{1,2}) >= 0);
  std::cout << "trie: all tests passed\n";
}
