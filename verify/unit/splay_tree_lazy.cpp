#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include "library/data_structure/splay_tree_lazy.hpp"

namespace {

struct SumNode {
  long long sum;
  int size;
  bool operator==(const SumNode&) const = default;
};

SumNode sum(SumNode x, SumNode y) { return {x.sum+y.sum,x.size+y.size}; }
SumNode zero() { return {0,0}; }
SumNode add_to_node(long long value, SumNode node) {
  node.sum += value*node.size;
  return node;
}
long long add_actions(long long x, long long y) { return x+y; }
long long no_action() { return 0; }

using Tree = soryuusi::splay_tree<
    SumNode,sum,zero,long long,add_to_node,add_actions,no_action>;

void check(Tree& tree, const std::vector<long long>& values) {
  assert(tree.size() == static_cast<int>(values.size()));
  const long long total = std::accumulate(values.begin(),values.end(),0LL);
  assert(tree.prod_all() == SumNode({total,static_cast<int>(values.size())}));
  for(int i = 0; i < static_cast<int>(values.size()); ++i) {
    assert(tree.get(i) == SumNode({values[i],1}));
  }
  for(int l = 0; l <= static_cast<int>(values.size()); ++l) {
    for(int r = l; r <= static_cast<int>(values.size()); ++r) {
      const long long expected =
          std::accumulate(values.begin()+l,values.begin()+r,0LL);
      assert(tree.prod(l,r) == SumNode({expected,r-l}));
    }
  }
}

}  // namespace

int main() {
  std::vector<long long> values = {1,2,3,4,5,6,7,8,9};
  std::vector<SumNode> nodes;
  for(long long value : values) nodes.push_back({value,1});
  Tree tree(nodes);
  check(tree,values);

  tree.apply(2,8,10);
  for(int i = 2; i < 8; ++i) values[i] += 10;
  check(tree,values);
  tree.flip(1,7);
  std::reverse(values.begin()+1,values.begin()+7);
  check(tree,values);
  tree.insert_at(4,{20,1});
  values.insert(values.begin()+4,20);
  check(tree,values);
  tree.erase_at(6);
  values.erase(values.begin()+6);
  check(tree,values);

  std::cout << "splay_tree_lazy: all tests passed\n";
}
