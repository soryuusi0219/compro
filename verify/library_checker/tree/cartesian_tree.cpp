#include "library/utility/fastio.hpp"

#include <vector>

#include "library/data_structure/cartesian_tree.hpp"

int main() {
  int n;
  soryuusi::cin >> n;
  std::vector<int> a(n);
  for(int& x : a) soryuusi::cin >> x;
  const auto [parent,left,right,root] = soryuusi::cartesian_tree(a);
  (void)left;
  (void)right;
  (void)root;
  for(int i = 0; i < n; ++i) {
    soryuusi::cout << (parent[i] == -1 ? i : parent[i]) << " \n"[i+1 == n];
  }
}
