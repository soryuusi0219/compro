#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include "library/data_structure/splay_tree.hpp"

namespace {

long long sum(long long x, long long y) { return x+y; }
long long zero() { return 0; }

void check(soryuusi::splay_tree<long long,sum,zero>& tree,
           const std::vector<long long>& values) {
  assert(tree.size() == static_cast<int>(values.size()));
  assert(tree.prod_all() == std::accumulate(values.begin(),values.end(),0LL));
  for(int i = 0; i < static_cast<int>(values.size()); ++i) {
    assert(tree.get(i) == values[i]);
  }
  for(int l = 0; l <= static_cast<int>(values.size()); ++l) {
    for(int r = l; r <= static_cast<int>(values.size()); ++r) {
      assert(tree.prod(l,r) ==
             std::accumulate(values.begin()+l,values.begin()+r,0LL));
    }
  }
}

}  // namespace

int main() {
  std::vector<long long> values = {1,2,3,4,5,6,7,8,9};
  soryuusi::splay_tree<long long,sum,zero> tree(values);
  check(tree,values);

  tree.flip(2,8);
  std::reverse(values.begin()+2,values.begin()+8);
  check(tree,values);
  tree.insert_at(4,20);
  values.insert(values.begin()+4,20);
  check(tree,values);
  tree.erase_at(6);
  values.erase(values.begin()+6);
  check(tree,values);

  std::cout << "splay_tree: all tests passed\n";
}
