#include "library/utility/fastio.hpp"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "library/data_structure/merge_sort_tree.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<int> previous(n, -1);
  std::unordered_map<int, int> last;
  for (int i = 0; i < n; ++i) {
    int value;
    soryuusi::cin >> value;
    if (auto it = last.find(value); it != last.end()) previous[i] = it->second;
    last[value] = i;
  }
  soryuusi::MergeSortTree<int> tree(previous);
  while (q--) {
    int left, right;
    soryuusi::cin >> left >> right;
    soryuusi::cout << tree.count_lt(left, right, left) << '\n';
  }
}
