#include "library/utility/fastio.hpp"
#include <iostream>
#include "library/tree/permutation_tree.hpp"

int main(){
  int n;
  soryuusi::cin >> n;
  std::vector<int> p(n);
  for(int i = 0; i < n; ++i) soryuusi::cin >> p[i];
  soryuusi::PermutationTree PT(p);
  auto& tree = PT.tree;
  soryuusi::cout << tree.size() << "\n";
  for(auto [p,l,r,type] : PT.tree) {
    soryuusi::cout << p << " " << l << " " << r-1 << (type ? " linear\n" : " prime\n");
  }
}
