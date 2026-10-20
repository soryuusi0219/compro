#include "library/utility/fastio.hpp"

#include <string>
#include <vector>

#include "library/string/aho_corasick.hpp"

int main() {
  int n;
  soryuusi::cin >> n;

  std::vector<std::string> patterns(n);
  std::size_t total_length = 1;
  for (auto& pattern : patterns) {
    soryuusi::cin >> pattern;
    total_length += pattern.size();
  }

  soryuusi::AhoCorasick<> aho(static_cast<int>(total_length));
  for (const auto& pattern : patterns) aho.insert(pattern);
  aho.build();

  soryuusi::cout << aho.size() << '\n';
  for (int v = 1; v < aho.size(); ++v) {
    soryuusi::cout << aho.par[v] << ' ' << aho.fail[v] << '\n';
  }
  for (int i = 0; i < n; ++i) {
    soryuusi::cout << aho.node(patterns[i]) << " \n"[i + 1 == n];
  }
}
