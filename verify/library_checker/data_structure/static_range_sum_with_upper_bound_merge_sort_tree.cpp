#include "library/utility/fastio.hpp"
#include <iostream>
#include <vector>

#include "library/data_structure/merge_sort_tree_sum.hpp"

int main() {
  int n, q;
  soryuusi::cin >> n >> q;
  std::vector<long long> values(n);
  for (long long& value : values) soryuusi::cin >> value;
  soryuusi::MergeSortTree_Sum<long long, long long> tree(values, values);
  while (q--) {
    int left, right;
    long long upper_bound;
    soryuusi::cin >> left >> right >> upper_bound;
    soryuusi::cout << tree.count_leq(left, right, upper_bound) << ' '
              << tree.sum_leq(left, right, upper_bound) << '\n';
  }
}
